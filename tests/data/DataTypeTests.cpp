#include "ml/data/DataType.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

namespace
{
	using ml::DataType;
	using ml::DataTypeKind;
	using ml::TimeUnit;

	/** @brief Reports a failed expectation by throwing an exception. */
	void require(bool condition, std::string_view message)
	{
		if (!condition)
		{
			throw std::runtime_error(std::string(message));
		}
	}

	/** @brief Requires an operation to throw std::invalid_argument. */
	template<typename Operation>
	void expectInvalidArgument(Operation&& operation, std::string_view message)
	{
		try
		{
			static_cast<void>(std::forward<Operation>(operation)());
		}
		catch (const std::invalid_argument&)
		{
			return;
		}
		catch (const std::exception& error)
		{
			throw std::runtime_error(
				std::string(message) + ": unexpected exception: " + error.what());
		}
		catch (...)
		{
			throw std::runtime_error(
				std::string(message) + ": unexpected nonstandard exception.");
		}

		throw std::runtime_error(
			std::string(message) + ": expected std::invalid_argument.");
	}

	/** @brief Returns a checked reference to a descriptor's expected specification. */
	template<typename Spec>
	const Spec& getSpec(const DataType& type, std::string_view context)
	{
		const auto* spec = std::get_if<Spec>(&type.getParameters());

		if (spec == nullptr)
		{
			throw std::runtime_error(
				std::string(context) + ": incorrect parameter alternative.");
		}

		return *spec;
	}

	/** @brief Associates a parameterless factory with its expected kind. */
	struct SimpleFactoryCase
	{
		std::string_view name; ///< Name used in failure messages.
		DataType(*create)(); ///< Factory being tested.
		DataTypeKind expected_kind; ///< Required kind of the returned descriptor.
	};

	constexpr std::array simpleFactories
	{
		SimpleFactoryCase{ "Int8", &DataType::Int8, DataTypeKind::INT8 },
		SimpleFactoryCase{ "Int16", &DataType::Int16, DataTypeKind::INT16 },
		SimpleFactoryCase{ "Int32", &DataType::Int32, DataTypeKind::INT32 },
		SimpleFactoryCase{ "Int64", &DataType::Int64, DataTypeKind::INT64 },
		SimpleFactoryCase{ "UInt8", &DataType::UInt8, DataTypeKind::UINT8 },
		SimpleFactoryCase{ "UInt16", &DataType::UInt16, DataTypeKind::UINT16 },
		SimpleFactoryCase{ "UInt32", &DataType::UInt32, DataTypeKind::UINT32 },
		SimpleFactoryCase{ "UInt64", &DataType::UInt64, DataTypeKind::UINT64 },
		SimpleFactoryCase{ "Float16", &DataType::Float16, DataTypeKind::FLOAT16 },
		SimpleFactoryCase{ "BFloat16", &DataType::BFloat16, DataTypeKind::BFLOAT16 },
		SimpleFactoryCase{ "Float32", &DataType::Float32, DataTypeKind::FLOAT32 },
		SimpleFactoryCase{ "Float64", &DataType::Float64, DataTypeKind::FLOAT64 },
		SimpleFactoryCase{ "Complex64", &DataType::Complex64, DataTypeKind::COMPLEX64 },
		SimpleFactoryCase{ "Complex128", &DataType::Complex128, DataTypeKind::COMPLEX128 },
		SimpleFactoryCase{ "Boolean", &DataType::Boolean, DataTypeKind::BOOLEAN },
		SimpleFactoryCase{ "String", &DataType::String, DataTypeKind::STRING },
		SimpleFactoryCase{ "Binary", &DataType::Binary, DataTypeKind::BINARY },
		SimpleFactoryCase{ "Date", &DataType::Date, DataTypeKind::DATE },
		SimpleFactoryCase{ "Null", &DataType::Null, DataTypeKind::NULL_VALUE }
	};

	/** @brief Describes a decimal factory and its precision limit. */
	struct DecimalFactoryCase
	{
		std::string_view name; ///< Name used in failure messages.
		DataType(*create)(std::int32_t, std::int32_t); ///< Factory being tested.
		DataTypeKind expected_kind; ///< Required kind of the returned descriptor.
		std::int32_t maximum_precision; ///< Largest supported decimal precision.
	};

	constexpr std::array decimalFactories
	{
		DecimalFactoryCase{ "Decimal32", &DataType::Decimal32, DataTypeKind::DECIMAL32, 9 },
		DecimalFactoryCase{ "Decimal64", &DataType::Decimal64, DataTypeKind::DECIMAL64, 18 },
		DecimalFactoryCase{ "Decimal128", &DataType::Decimal128, DataTypeKind::DECIMAL128, 38 },
		DecimalFactoryCase{ "Decimal256", &DataType::Decimal256, DataTypeKind::DECIMAL256, 76 }
	};

	constexpr std::array validTimeUnits
	{
		TimeUnit::SECOND,
		TimeUnit::MILLISECOND,
		TimeUnit::MICROSECOND,
		TimeUnit::NANOSECOND,
		TimeUnit::PICOSECOND,
		TimeUnit::FEMTOSECOND
	};

	/** @brief Checks every parameterless factory's kind and empty specification. */
	void testParameterlessFactories()
	{
		for (const auto& factory : simpleFactories)
		{
			const auto type = factory.create();
			const std::string context(factory.name);

			require(type.getKind() == factory.expected_kind,
				context + ": incorrect kind.");

			require(std::holds_alternative<std::monostate>(type.getParameters()),
				context + ": expected std::monostate.");
		}
	}

	/** @brief Checks valid decimal precisions and preservation of signed scales. */
	void testValidDecimals()
	{
		for (const auto& factory : decimalFactories)
		{
			const std::array<std::int32_t, 3> precisions
			{
				1,
				factory.maximum_precision / 2,
				factory.maximum_precision
			};

			const std::array<std::int32_t, 6> scales
			{
				std::numeric_limits<std::int32_t>::min(),
				-2,
				0,
				2,
				factory.maximum_precision + 1,
				std::numeric_limits<std::int32_t>::max()
			};

			for (const auto precision : precisions)
			{
				for (const auto scale : scales)
				{
					const std::string context = std::string(factory.name)
						+ "(precision=" + std::to_string(precision)
						+ ", scale=" + std::to_string(scale) + ")";

					const auto type = factory.create(precision, scale);

					require(type.getKind() == factory.expected_kind,
						context + ": incorrect kind.");

					const auto& spec = getSpec<ml::DecimalSpec>(type, context);

					require(spec.precision == precision,
						context + ": precision was not preserved.");

					require(spec.scale == scale,
						context + ": scale was not preserved.");
				}
			}
		}
	}

	/** @brief Checks rejection of precision values outside each decimal format's range. */
	void testInvalidDecimalPrecisions()
	{
		for (const auto& factory : decimalFactories)
		{
			const std::array<std::int32_t, 5> invalidPrecisions
			{
				std::numeric_limits<std::int32_t>::min(),
				-1,
				0,
				factory.maximum_precision + 1,
				std::numeric_limits<std::int32_t>::max()
			};

			for (const auto precision : invalidPrecisions)
			{
				const auto create = factory.create;
				const std::string context = std::string(factory.name)
					+ "(precision=" + std::to_string(precision) + ")";

				expectInvalidArgument(
					[create, precision]
					{
						return create(precision, 0);
					},
					context);
			}
		}
	}

	/** @brief Checks fixed-binary descriptor lengths without allocating byte buffers. */
	void testFixedBinary()
	{
		const std::array<std::size_t, 4> byteCounts
		{
			0,
			1,
			16,
			std::numeric_limits<std::size_t>::max()
		};

		for (const auto byteCount : byteCounts)
		{
			const std::string context =
				"FixedBinary(" + std::to_string(byteCount) + ")";

			const auto type = DataType::FixedBinary(byteCount);

			require(type.getKind() == DataTypeKind::FIXED_BINARY,
				context + ": incorrect kind.");

			const auto& spec = getSpec<ml::FixedBinarySpec>(type, context);

			require(spec.byte_count == byteCount,
				context + ": byte count was not preserved.");
		}
	}

	/** @brief Checks all supported units for time, timestamp, and duration factories. */
	void testValidTemporalUnits()
	{
		for (const auto unit : validTimeUnits)
		{
			const std::string suffix =
				"(unit=" + std::to_string(static_cast<int>(unit)) + ")";

			const auto time = DataType::Time(unit);
			require(time.getKind() == DataTypeKind::TIME,
				"Time" + suffix + ": incorrect kind.");
			require(getSpec<ml::TimeSpec>(time, "Time" + suffix).unit == unit,
				"Time" + suffix + ": unit was not preserved.");

			const auto timestamp = DataType::Timestamp(unit);
			require(timestamp.getKind() == DataTypeKind::TIMESTAMP,
				"Timestamp" + suffix + ": incorrect kind.");
			require(getSpec<ml::TimestampSpec>(timestamp, "Timestamp" + suffix).unit == unit,
				"Timestamp" + suffix + ": unit was not preserved.");

			const auto duration = DataType::Duration(unit);
			require(duration.getKind() == DataTypeKind::DURATION,
				"Duration" + suffix + ": incorrect kind.");
			require(getSpec<ml::DurationSpec>(duration, "Duration" + suffix).unit == unit,
				"Duration" + suffix + ": unit was not preserved.");
		}
	}

	/** @brief Checks rejection of undeclared enum values by every temporal factory. */
	void testInvalidTemporalUnits()
	{
		constexpr std::array invalidUnits
		{
			static_cast<TimeUnit>(-1),
			static_cast<TimeUnit>(999)
		};

		for (const auto unit : invalidUnits)
		{
			const std::string suffix =
				"(unit=" + std::to_string(static_cast<int>(unit)) + ")";

			expectInvalidArgument(
				[unit] { return DataType::Time(unit); },
				"Time" + suffix);

			expectInvalidArgument(
				[unit] { return DataType::Timestamp(unit); },
				"Timestamp" + suffix);

			expectInvalidArgument(
				[unit]
				{
					return DataType::Timestamp(
						unit, std::optional<std::string>{ "UTC" });
				},
				"Timestamp with UTC" + suffix);

			expectInvalidArgument(
				[unit] { return DataType::Duration(unit); },
				"Duration" + suffix);
		}
	}

	/** @brief Checks unspecified, named, and explicitly empty timestamp time zones. */
	void testTimestampTimeZones()
	{
		const auto defaultTimestamp = DataType::Timestamp(TimeUnit::NANOSECOND);
		const auto& defaultSpec =
			getSpec<ml::TimestampSpec>(defaultTimestamp, "Default timestamp");

		require(!defaultSpec.time_zone.has_value(),
			"An omitted time zone must remain unspecified.");

		const auto naiveTimestamp =
			DataType::Timestamp(TimeUnit::NANOSECOND, std::nullopt);
		const auto& naiveSpec =
			getSpec<ml::TimestampSpec>(naiveTimestamp, "Explicitly naive timestamp");

		require(!naiveSpec.time_zone.has_value(),
			"An explicit std::nullopt must remain unspecified.");

		constexpr std::array zoneNames
		{
			std::string_view{ "UTC" },
			std::string_view{ "America/New_York" },
			std::string_view{ "America/Argentina/Buenos_Aires" }
		};

		for (const auto zone : zoneNames)
		{
			const std::string context = "Timestamp zone " + std::string(zone);

			const auto timestamp = DataType::Timestamp(
				TimeUnit::NANOSECOND,
				std::optional<std::string>{ std::string(zone) });

			const auto& spec = getSpec<ml::TimestampSpec>(timestamp, context);

			require(spec.time_zone.has_value(),
				context + ": time zone was lost.");

			require(*spec.time_zone == zone,
				context + ": time zone was not preserved.");
		}

		expectInvalidArgument(
			[]
			{
				return DataType::Timestamp(
					TimeUnit::NANOSECOND,
					std::optional<std::string>{ std::string{} });
			},
			"Timestamp with an explicitly empty time zone");
	}

	/** @brief Associates a readable test-group name with its test function. */
	struct TestCase
	{
		std::string_view name; ///< Name displayed in the test output.
		void (*run)(); ///< Test function to execute.
	};
}

/** @brief Runs every test group and returns a status suitable for CTest. */
int main()
{
	const std::array tests
	{
		TestCase{ "Parameterless factories", testParameterlessFactories },
		TestCase{ "Valid decimal parameters", testValidDecimals },
		TestCase{ "Invalid decimal precisions", testInvalidDecimalPrecisions },
		TestCase{ "Fixed-binary lengths", testFixedBinary },
		TestCase{ "Valid temporal units", testValidTemporalUnits },
		TestCase{ "Invalid temporal units", testInvalidTemporalUnits },
		TestCase{ "Timestamp time zones", testTimestampTimeZones }
	};

	std::size_t failures = 0;

	for (const auto& test : tests)
	{
		try
		{
			test.run();
			std::cout << "[PASS] " << test.name << '\n';
		}
		catch (const std::exception& error)
		{
			++failures;
			std::cerr << "[FAIL] " << test.name << ": " << error.what() << '\n';
		}
		catch (...)
		{
			++failures;
			std::cerr << "[FAIL] " << test.name << ": unknown exception.\n";
		}
	}

	std::cout << tests.size() - failures << '/' << tests.size()
		<< " test groups passed.\n";

	return failures == 0 ? 0 : 1;
}