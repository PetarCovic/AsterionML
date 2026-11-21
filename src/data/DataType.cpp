#include "ml/data/DataType.hpp"

#include <utility>
#include <stdexcept>

namespace ml
{
	DataType::DataType(DataTypeKind kind, std::variant<std::monostate, DecimalSpec, FixedBinarySpec, TimeSpec, TimestampSpec, DurationSpec> parameters)
		: kind_(kind), parameters_(std::move(parameters))
	{
		switch (kind)
		{
		case DataTypeKind::DECIMAL32:
		{
			const auto* spec = std::get_if<DecimalSpec>(&parameters_);

			if (spec == nullptr)
			{
				throw std::invalid_argument("DECIMAL32 requires DecimalSpec.");
			}

			if (spec->precision < 1 || spec->precision > 9)
			{
				throw std::invalid_argument(
					"DECIMAL32 precision must be between 1 and 9.");
			}

			break;
		}
		case DataTypeKind::DECIMAL64:
		{
			const auto* spec = std::get_if<DecimalSpec>(&parameters_);

			if (spec == nullptr)
			{
				throw std::invalid_argument("DECIMAL64 requires DecimalSpec.");
			}

			if (spec->precision < 1 || spec->precision > 18)
			{
				throw std::invalid_argument(
					"DECIMAL64 precision must be between 1 and 18.");
			}

			break;
		}
		case DataTypeKind::DECIMAL128:
		{
			const auto* spec = std::get_if<DecimalSpec>(&parameters_);

			if (spec == nullptr)
			{
				throw std::invalid_argument("DECIMAL128 requires DecimalSpec.");
			}

			if (spec->precision < 1 || spec->precision > 38)
			{
				throw std::invalid_argument(
					"DECIMAL128 precision must be between 1 and 38.");
			}

			break;
		}
		case DataTypeKind::DECIMAL256:
		{
			const auto* spec = std::get_if<DecimalSpec>(&parameters_);

			if (spec == nullptr)
			{
				throw std::invalid_argument("DECIMAL256 requires DecimalSpec.");
			}

			if (spec->precision < 1 || spec->precision > 76)
			{
				throw std::invalid_argument(
					"DECIMAL256 precision must be between 1 and 76.");
			}

			break;
		}
		case DataTypeKind::FIXED_BINARY:
		{
			if (!std::holds_alternative<FixedBinarySpec>(parameters_))
			{
				throw std::invalid_argument(
					"FIXED_BINARY requires FixedBinarySpec.");
			}

			break;
		}
		case DataTypeKind::TIME:
		{
			const auto* spec = std::get_if<TimeSpec>(&parameters_);

			if (spec == nullptr)
			{
				throw std::invalid_argument("TIME requires TimeSpec.");
			}

			validateTimeUnit(spec->unit);

			break;
		}
		case DataTypeKind::TIMESTAMP:
		{
			const auto* spec = std::get_if<TimestampSpec>(&parameters_);

			if (spec == nullptr)
			{
				throw std::invalid_argument("TIMESTAMP requires TimestampSpec.");
			}

			if (spec->time_zone.has_value() && spec->time_zone->empty())
			{
				throw std::invalid_argument("TIMESTAMP time zone cannot be empty.");
			}

			validateTimeUnit(spec->unit);

			break;
		}
		case DataTypeKind::DURATION:
		{
			const auto* spec = std::get_if<DurationSpec>(&parameters_);

			if (spec == nullptr)
			{
				throw std::invalid_argument("DURATION requires DurationSpec.");
			}

			validateTimeUnit(spec->unit);

			break;
		}
		case DataTypeKind::INT8:
		case DataTypeKind::INT16:
		case DataTypeKind::INT32:
		case DataTypeKind::INT64:
		case DataTypeKind::UINT8:
		case DataTypeKind::UINT16:
		case DataTypeKind::UINT32:
		case DataTypeKind::UINT64:
		case DataTypeKind::FLOAT16:
		case DataTypeKind::BFLOAT16:
		case DataTypeKind::FLOAT32:
		case DataTypeKind::FLOAT64:
		case DataTypeKind::COMPLEX64:
		case DataTypeKind::COMPLEX128:
		case DataTypeKind::STRING:
		case DataTypeKind::BINARY:
		case DataTypeKind::BOOLEAN:
		case DataTypeKind::DATE:
		case DataTypeKind::NULL_VALUE:
		{
			const auto* spec = std::get_if<std::monostate>(&parameters_);

			if (spec == nullptr)
			{
				throw std::invalid_argument("Parameterless data types require std::monostate.");
			}

			break;
		}
		default:
			throw std::invalid_argument("Unrecognized data type kind.");
		}
	}

	/**
	* @brief Creates an Int8 data type.
	*/
	[[nodiscard]] DataType DataType::Int8()
	{
		return DataType(DataTypeKind::INT8, std::monostate{});
	}

	/**
	* @brief Creates an Int16 data type.
	*/
	[[nodiscard]] DataType DataType::Int16()
	{
		return DataType(DataTypeKind::INT16, std::monostate{});
	}

	/**
	* @brief Creates an Int32 data type.
	*/
	[[nodiscard]] DataType DataType::Int32()
	{
		return DataType(DataTypeKind::INT32, std::monostate{});
	}

	/**
	* @brief Creates an Int64 data type.
	*/
	[[nodiscard]] DataType DataType::Int64()
	{
		return DataType(DataTypeKind::INT64, std::monostate{});
	}

	/**
	* @brief Creates an UInt8 data type.
	*/
	[[nodiscard]] DataType DataType::UInt8()
	{
		return DataType(DataTypeKind::UINT8, std::monostate{});
	}

	/**
	* @brief Creates an UInt16 data type.
	*/
	[[nodiscard]] DataType DataType::UInt16()
	{
		return DataType(DataTypeKind::UINT16, std::monostate{});
	}

	/**
	* @brief Creates an UInt32 data type.
	*/
	[[nodiscard]] DataType DataType::UInt32()
	{
		return DataType(DataTypeKind::UINT32, std::monostate{});
	}

	/**
	* @brief Creates an UInt64 data type.
	*/
	[[nodiscard]] DataType DataType::UInt64()
	{
		return DataType(DataTypeKind::UINT64, std::monostate{});
	}

	/**
	* @brief Creates a Float16 data type.
	*/
	[[nodiscard]] DataType DataType::Float16()
	{
		return DataType(DataTypeKind::FLOAT16, std::monostate{});
	}

	/**
	* @brief Creates a BFloat16 data type.
	*/
	[[nodiscard]] DataType DataType::BFloat16()
	{
		return DataType(DataTypeKind::BFLOAT16, std::monostate{});
	}

	/**
	* @brief Creates a Float32 data type.
	*/
	[[nodiscard]] DataType DataType::Float32()
	{
		return DataType(DataTypeKind::FLOAT32, std::monostate{});
	}

	/** 
	* @brief Creates a Float64 data type.
	*/
	[[nodiscard]] DataType DataType::Float64()
	{
		return DataType(DataTypeKind::FLOAT64, std::monostate{});
	}
	
	/**
	* @brief Creates a Complex64 data type.
	*/
	[[nodiscard]] DataType DataType::Complex64()
	{
		return DataType(DataTypeKind::COMPLEX64, std::monostate{});
	}

	/**
	* @brief Creates a Complex128 data type.
	*/
	[[nodiscard]] DataType DataType::Complex128()
	{
		return DataType(DataTypeKind::COMPLEX128, std::monostate{});
	}

	/**
	* @brief Creates a Boolean data type.
	*/
	[[nodiscard]] DataType DataType::Boolean()
	{
		return DataType(DataTypeKind::BOOLEAN, std::monostate{});
	}

	/**
	* @brief Creates a String data type.
	*/
	[[nodiscard]] DataType DataType::String()
	{
		return DataType(DataTypeKind::STRING, std::monostate{});
	}

	/**
	* @brief Creates a Binary data type.
	*/
	[[nodiscard]] DataType DataType::Binary()
	{
		return DataType(DataTypeKind::BINARY, std::monostate{});
	}

	/**
	* @brief Creates a Date data type.
	*/
	[[nodiscard]] DataType DataType::Date()
	{
		return DataType(DataTypeKind::DATE, std::monostate{});
	}

	/**
	* @brief Creates a Null data type.
	*/
	[[nodiscard]] DataType DataType::Null()
	{
		return DataType(DataTypeKind::NULL_VALUE, std::monostate{});
	}

	/**
	* @brief Creates a Decimal32 data type with the specified precision and scale.
	*/
	[[nodiscard]] DataType DataType::Decimal32(std::int32_t precision, std::int32_t scale)
	{
		return DataType(DataTypeKind::DECIMAL32, DecimalSpec{ precision, scale });
	}

	/**
	* @brief Creates a Decimal64 data type with the specified precision and scale.
	*/
	[[nodiscard]] DataType DataType::Decimal64(std::int32_t precision, std::int32_t scale)
	{
		return DataType(DataTypeKind::DECIMAL64, DecimalSpec{ precision, scale });
	}

	/**
	* @brief Creates a Decimal128 data type with the specified precision and scale.
	*/
	[[nodiscard]] DataType DataType::Decimal128(std::int32_t precision, std::int32_t scale)
	{
		return DataType(DataTypeKind::DECIMAL128, DecimalSpec{ precision, scale });
	}

	/**
	* @brief Creates a Decimal256 data type with the specified precision and scale.
	*/
	[[nodiscard]] DataType DataType::Decimal256(std::int32_t precision, std::int32_t scale)
	{
		return DataType(DataTypeKind::DECIMAL256, DecimalSpec{ precision, scale });
	}

	/**
	* @brief Creates a FixedBinary data type with the specified byte count.
	*/
	[[nodiscard]] DataType DataType::FixedBinary(std::size_t byte_count)
	{
		return DataType(DataTypeKind::FIXED_BINARY, FixedBinarySpec{ byte_count });
	}

	/**
	* @brief Creates a Time data type with the specified time unit.
	*/
	[[nodiscard]] DataType DataType::Time(TimeUnit unit)
	{
		return DataType(DataTypeKind::TIME, TimeSpec{ unit });
	}

	/**
	* @brief Creates a Timestamp data type with the specified time unit and optional timezone.
	*/
	[[nodiscard]] DataType DataType::Timestamp(TimeUnit unit, std::optional<std::string> time_zone)
	{
		return DataType(DataTypeKind::TIMESTAMP, TimestampSpec{ unit, std::move(time_zone) });
	}

	/**
	* @brief Creates a Duration data type with the specified time unit.
	*/
	[[nodiscard]] DataType DataType::Duration(TimeUnit unit)
	{
		return DataType(DataTypeKind::DURATION, DurationSpec{ unit });
	}

	/**
	* @brief Returns the kind of the data type.
	*/
	[[nodiscard]] DataTypeKind DataType::getKind() const noexcept
	{
		return kind_;
	}

	/**
	* @brief Returns a borrowed, read-only reference to the parameters of the data type.
	*/
	[[nodiscard]] const std::variant<std::monostate, DecimalSpec, FixedBinarySpec, TimeSpec, TimestampSpec, DurationSpec>& DataType::getParameters() const noexcept
	{
		return parameters_;
	}

	/**
	* @brief  Checks if the data type is a signed integer.
	*/
	[[nodiscard]] bool DataType::isSignedInteger() const noexcept
	{
		return kind_ == DataTypeKind::INT8 || kind_ == DataTypeKind::INT16 ||
			kind_ == DataTypeKind::INT32 || kind_ == DataTypeKind::INT64;
	}

	/**
	* @brief Checks if the data type is an unsigned integer.
	*/
	[[nodiscard]] bool DataType::isUnsignedInteger() const noexcept
	{
		return  kind_ == DataTypeKind::UINT8 || kind_ == DataTypeKind::UINT16 ||
			kind_ == DataTypeKind::UINT32 || kind_ == DataTypeKind::UINT64;
	}

	/**
	* @brief Checks if the data type is an integer.
	*/
	[[nodiscard]] bool DataType::isInteger() const noexcept
	{
		return isSignedInteger() || isUnsignedInteger();
	}

	/**
	* @brief Checks if the data type is a floating-point number.
	*/
	[[nodiscard]] bool DataType::isFloatingPoint() const noexcept
	{
		return kind_ == DataTypeKind::FLOAT16 || kind_ == DataTypeKind::BFLOAT16 ||
			kind_ == DataTypeKind::FLOAT32 || kind_ == DataTypeKind::FLOAT64;
	}

	/**
	* @brief Checks if the data type is a complex number.
	*/
	[[nodiscard]] bool DataType::isComplex() const noexcept
	{
		return kind_ == DataTypeKind::COMPLEX64 || kind_ == DataTypeKind::COMPLEX128;
	}

	/**
	* @brief Checks if the data type is a decimal number.
	*/
	[[nodiscard]] bool DataType::isDecimal() const noexcept
	{
		return kind_ == DataTypeKind::DECIMAL32 || kind_ == DataTypeKind::DECIMAL64 ||
			kind_ == DataTypeKind::DECIMAL128 || kind_ == DataTypeKind::DECIMAL256;
	}

	/**
	* @brief Checks if the data type is an integer, floating-point, or decimal number.
	*/
	[[nodiscard]] bool DataType::isRealNumeric() const noexcept
	{
		return  isInteger() || isFloatingPoint() || isDecimal();
	}

	/**
	* @brief Checks if the data type is an integer or decimal number.
	*/
	[[nodiscard]] bool DataType::isExactNumeric() const noexcept
	{
		return isInteger() || isDecimal();
	}

	/**
	* @brief Checks if the data type is a floating-point or complex number.
	*/
	[[nodiscard]] bool DataType::isInexactNumeric() const noexcept
	{
		return isFloatingPoint() || isComplex();
	}

	/**
	* @brief Checks if the data type is an integer, floating-point, complex, or decimal number.
	*/
	[[nodiscard]] bool DataType::isNumeric() const noexcept
	{
		return isInteger() || isFloatingPoint() || isComplex() || isDecimal();
	}

	/**
	* @brief Checks if the data type is a string.
	*/
	[[nodiscard]] bool DataType::isString() const noexcept
	{
		return kind_ == DataTypeKind::STRING;
	}

	/**
	* @brief Checks if the data type is a variable-length or fixed-length binary type.
	*/
	[[nodiscard]] bool DataType::isBinary() const noexcept
	{
		return kind_ == DataTypeKind::BINARY || kind_ == DataTypeKind::FIXED_BINARY;
	}

	/**
	* @brief Checks if the data type is a variable-length binary type.
	*/
	[[nodiscard]] bool DataType::isVariableBinary() const noexcept
	{
		return kind_ == DataTypeKind::BINARY;
	}

	/**
	* @brief Checks if the data type is a fixed-length binary type.
	*/
	[[nodiscard]] bool DataType::isFixedBinary() const noexcept
	{
		return kind_ == DataTypeKind::FIXED_BINARY;
	}

	/**
	* @brief Checks if the data type is a boolean type.
	*/
	[[nodiscard]] bool DataType::isBoolean() const noexcept
	{
		return kind_ == DataTypeKind::BOOLEAN;
	}

	/**
	* @brief Checks if the data type is a null type.
	*/
	[[nodiscard]] bool DataType::isNull() const noexcept
	{
		return kind_ == DataTypeKind::NULL_VALUE;
	}

	/**
	* @brief Checks if the data type is a date, time, timestamp, or duration.
	*/
	[[nodiscard]] bool DataType::isTemporal() const noexcept
	{
		return kind_ == DataTypeKind::DATE || kind_ == DataTypeKind::TIME ||
			kind_ == DataTypeKind::TIMESTAMP || kind_ == DataTypeKind::DURATION;
	}

	/**
	* @brief Checks if the data type is a date type.
	*/
	[[nodiscard]] bool DataType::isDate() const noexcept
	{
		return kind_ == DataTypeKind::DATE;
	}

	/**
	* @brief Checks if the data type is a time type.
	*/
	[[nodiscard]] bool DataType::isTime() const noexcept
	{
		return kind_ == DataTypeKind::TIME;
	}

	/**
	* @brief Checks if the data type is a timestamp type.
	*/
	[[nodiscard]] bool DataType::isTimestamp() const noexcept
	{
		return kind_ == DataTypeKind::TIMESTAMP;
	}

	/**
	* @brief Checks if the data type is a duration type.
	*/
	[[nodiscard]] bool DataType::isDuration() const noexcept
	{
		return kind_ == DataTypeKind::DURATION;
	}

	/**
	* @brief Checks if the data type is an 8-bit signed integer type.
	*/
	[[nodiscard]] bool DataType::isInt8() const noexcept
	{
		return  kind_ == DataTypeKind::INT8;
	}

	/**
	* @brief Checks if the data type is a 16-bit signed integer type.
	*/
	[[nodiscard]] bool DataType::isInt16() const noexcept
	{
		return   kind_ == DataTypeKind::INT16;
	}

	/**
	* @brief Checks if the data type is a 32-bit signed integer type.
	*/
	[[nodiscard]] bool DataType::isInt32() const noexcept
	{
		return   kind_ == DataTypeKind::INT32;
	}

	/**
	* @brief Checks if the data type is a 64-bit signed integer type.
	*/
	[[nodiscard]] bool DataType::isInt64() const noexcept
	{
		return   kind_ == DataTypeKind::INT64;
	}

	/**
	* @brief Checks if the data type is an unsigned 8-bit integer type.
	*/
	[[nodiscard]] bool DataType::isUInt8() const noexcept
	{
		return kind_ == DataTypeKind::UINT8;
	}

	/**
	* @brief Checks if the data type is an unsigned 16-bit integer type.
	*/
	[[nodiscard]] bool DataType::isUInt16() const noexcept
	{
		return kind_ == DataTypeKind::UINT16;
	}

	/**
	* @brief Checks if the data type is an unsigned 32-bit integer type.
	*/
	[[nodiscard]] bool DataType::isUInt32() const noexcept
	{
		return kind_ == DataTypeKind::UINT32;
	}

	/**
	* @brief Checks if the data type is an unsigned 64-bit integer type.
	*/
	[[nodiscard]] bool DataType::isUInt64() const noexcept
	{
		return kind_ == DataTypeKind::UINT64;
	}

	/**
	* @brief Checks if the data type is a 16-bit IEEE 754 floating-point type.
	*/
	[[nodiscard]] bool DataType::isFloat16() const noexcept
	{
		return kind_ == DataTypeKind::FLOAT16;
	}

	/**
	* @brief Checks if the data type is a bfloat16 type.
	*/
	[[nodiscard]] bool DataType::isBFloat16() const noexcept
	{
		return kind_ == DataTypeKind::BFLOAT16;
	}

	/**
	* @brief Checks if the data type is a 32-bit IEEE 754 floating-point type.
	*/
	[[nodiscard]] bool DataType::isFloat32() const noexcept
	{
		return  kind_ == DataTypeKind::FLOAT32;
	}

	/**
	* @brief Checks if the data type is a 64-bit IEEE 754 floating-point type.
	*/
	[[nodiscard]] bool DataType::isFloat64() const noexcept
	{
		return kind_ == DataTypeKind::FLOAT64;
	}

	/**
	* @brief Checks if the data type is a complex number with 32-bit components.
	*/
	[[nodiscard]] bool DataType::isComplex64() const noexcept
	{
		return kind_ == DataTypeKind::COMPLEX64;
	}

	/**
	* @brief Checks if the data type is a complex number with 64-bit components.
	*/
	[[nodiscard]] bool DataType::isComplex128() const noexcept
	{
		return kind_ == DataTypeKind::COMPLEX128;
	}

	/**
	* @brief Checks if the data type is a Decimal32 type.
	*/
	[[nodiscard]] bool DataType::isDecimal32() const noexcept
	{
		return kind_ == DataTypeKind::DECIMAL32;
	}

	/**
	* @brief Checks if the data type is a Decimal64 type.
	*/
	[[nodiscard]] bool DataType::isDecimal64() const noexcept
	{
		return kind_ == DataTypeKind::DECIMAL64;
	}

	/**
	* @brief Checks if the data type is a Decimal128 type.
	*/
	[[nodiscard]] bool DataType::isDecimal128() const noexcept
	{
		return kind_ == DataTypeKind::DECIMAL128;
	}

	/**
	* @brief Checks if the data type is a Decimal256 type.
	*/
	[[nodiscard]] bool DataType::isDecimal256() const noexcept
	{
		return kind_ == DataTypeKind::DECIMAL256;
	}

	/**
	* @brief Checks if the data type has associated parameters.
	*/
	[[nodiscard]] bool DataType::hasParameters() const noexcept
	{
		return !std::holds_alternative<std::monostate>(parameters_);
	}

	/**
	* @brief Checks if the data type has an associated time unit. Recognizes TIME, TIMESTAMP, and DURATION types.
	*/
	[[nodiscard]] bool DataType::hasTimeUnit() const noexcept
	{
		return  kind_ == DataTypeKind::TIME || kind_ == DataTypeKind::TIMESTAMP ||
			kind_ == DataTypeKind::DURATION;
	}

	/**
	* @brief Checks if the data type is a timezone-aware timestamp.
	*/
	[[nodiscard]] bool DataType::isTimezoneAware() const noexcept
	{
		return kind_ == DataTypeKind::TIMESTAMP &&
			std::holds_alternative<TimestampSpec>(parameters_) &&
			std::get<TimestampSpec>(parameters_).time_zone.has_value();
	}

	/**
	* @brief Checks if the data type is a timezone-naive timestamp.
	*/
	[[nodiscard]] bool DataType::isTimezoneNaive() const noexcept
	{
		return kind_ == DataTypeKind::TIMESTAMP &&
			std::holds_alternative<TimestampSpec>(parameters_) &&
			!std::get<TimestampSpec>(parameters_).time_zone.has_value();
	}

	/**
	* @brief Validates the provided time unit. Throws an exception if the time unit is unrecognized.
	*/
	void DataType::validateTimeUnit(TimeUnit unit)
	{
		switch (unit)
		{
		case TimeUnit::SECOND:
		case TimeUnit::MILLISECOND:
		case TimeUnit::MICROSECOND:
		case TimeUnit::NANOSECOND:
		case TimeUnit::PICOSECOND:
		case TimeUnit::FEMTOSECOND:
			return;

		default:
			throw std::invalid_argument("Unrecognized time unit.");
		}
	}
}