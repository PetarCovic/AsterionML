#pragma once

#include <variant>
#include <cstdint>
#include <cstddef>
#include <optional>
#include <string>

namespace ml
{
	/**
	* @brief 
	* Identifies the logical element kind represented by a DataType descriptor.
	*/
	enum class DataTypeKind
	{
		INT8,			///< Signed 8-bit integer.
		INT16,			///< Signed 16-bit integer.
		INT32,			///< Signed 32-bit integer.
		INT64,			///< Signed 64-bit integer.
		UINT8,			///< Unsigned 8-bit integer.
		UINT16,			///< Unsigned 16-bit integer.
		UINT32,			///< Unsigned 32-bit integer.
		UINT64,			///< Unsigned 64-bit integer.
		FLOAT16,		///< 16-bit floating-point number using the IEEE 754 binary16 format (half precision).
		BFLOAT16,		///< 16-bit bfloat16 number with 8 exponent bits and 7 fraction bits.
		FLOAT32,		///< 32-bit floating-point number using the IEEE 754 binary32 format (single precision).
		FLOAT64,		///< 64-bit floating-point number using the IEEE 754 binary64 format (double precision).
		COMPLEX64,		///< Two 32-bit floating-point components: real and imaginary.
		COMPLEX128,		///< Two 64-bit floating-point components.
		DECIMAL32,		///< 32-bit Fixed-point decimal values with declared precision and scale.
		DECIMAL64,		///< 64-bit Fixed-point decimal values with declared precision and scale.
		DECIMAL128,		///< 128-bit Fixed-point decimal values with declared precision and scale.
		DECIMAL256,		///< 256-bit Fixed-point decimal values with declared precision and scale.
		STRING,			///< Variable-length UTF-8 text.
		BINARY,			///< Variable-length sequence of arbitrary bytes.
		FIXED_BINARY,	///< Byte sequence whose length is declared by the descriptor.
		BOOLEAN,		///< True or false.
		DATE,			///< Calendar date.
		TIME,			///< Time of day.
		TIMESTAMP,		///< Date and time.
		DURATION,		///< Elapsed time measured in ticks of a declared unit.
		NULL_VALUE		///< Identifies an all-null type whose only permitted value is null.
	};

	/**
	 * @brief
	 * Specifies the duration of one tick for time, timestamp, and duration values.
	 */
	enum class TimeUnit
	{
		SECOND,			///< One second per tick
		MILLISECOND,	///< One thousandth of a second per tick.
		MICROSECOND,	///< One millionth of a second per tick.
		NANOSECOND,		///< One billionth of a second per tick.
		PICOSECOND,		///< One trillionth of a second per tick.
		FEMTOSECOND,	///< One quadrillionth of a second per tick.
	};

	/**
	* @brief
	* Describes the parameters of a decimal type, including its precision and scale.
	*/
	struct DecimalSpec
	{
		std::int32_t precision; ///< Maximum permitted number of significant digits.
		std::int32_t scale; ///< Value=unscaled_value * 10^(-scale).
	};

	/**
	 * @brief
	 * Describes the parameters of a fixed-size binary type.
	 */
	struct FixedBinarySpec
	{
		std::size_t byte_count; ///< Declared length of the binary sequence in bytes.
	};

	/**
	 * @brief
	 * Describes the parameters of a time type.
	 */
	struct TimeSpec
	{
		TimeUnit unit; ///< Duration of one tick.
	};

	/**
	 * @brief
	 * Describes the parameters of a timestamp type. Absent timezone value means time_zone is unspecified.
	 */
	struct TimestampSpec
	{
		TimeUnit unit; ///< Duration of one tick.
		std::optional<std::string> time_zone; ///< Optional IANA timezone name, such as "America/New_York" or "UTC". If absent, the timestamp is considered timezone-naive.
	};

	/**
	 * @brief
	 * Describes the parameters of a duration type.
	 */
	struct DurationSpec
	{
		TimeUnit unit; ///< Duration of one tick.
	};
	
	/**
	 * @brief
	 * Represents the type of a data element.
	 */
	class DataType
	{
	public:
		[[nodiscard]] static DataType Int8(); ///< @brief Creates an Int8 data type.
		[[nodiscard]] static DataType Int16(); ///< @brief Creates an Int16 data type.
		[[nodiscard]] static DataType Int32(); ///< @brief Creates an Int32 data type.
		[[nodiscard]] static DataType Int64(); ///< @brief Creates an Int64 data type.
		[[nodiscard]] static DataType UInt8(); ///< @brief Creates an UInt8 data type.
		[[nodiscard]] static DataType UInt16(); ///< @brief Creates an UInt16 data type.
		[[nodiscard]] static DataType UInt32(); ///< @brief Creates an UInt32 data type.
		[[nodiscard]] static DataType UInt64(); ///< @brief Creates an UInt64 data type.
		[[nodiscard]] static DataType Float16(); ///< @brief Creates a Float16 data type.
		[[nodiscard]] static DataType BFloat16(); ///< @brief Creates a BFloat16 data type.
		[[nodiscard]] static DataType Float32(); ///< @brief Creates a Float32 data type.
		[[nodiscard]] static DataType Float64(); ///< @brief Creates a Float64 data type.
		[[nodiscard]] static DataType Complex64(); ///< @brief Creates a Complex64 data type.
		[[nodiscard]] static DataType Complex128(); ///< @brief Creates a Complex128 data type.
		[[nodiscard]] static DataType Boolean(); ///< @brief Creates a Boolean data type.
		[[nodiscard]] static DataType String(); ///< @brief Creates a String data type.
		[[nodiscard]] static DataType Binary(); ///< @brief Creates a Binary data type.
		[[nodiscard]] static DataType Date(); ///< @brief Creates a Date data type.
		[[nodiscard]] static DataType Null(); ///< @brief Creates a Null data type.
		[[nodiscard]] static DataType Decimal32(std::int32_t precision, std::int32_t scale); ///< @brief Creates a Decimal32 data type with the specified precision and scale.
		[[nodiscard]] static DataType Decimal64(std::int32_t precision, std::int32_t scale); ///< @brief Creates a Decimal64 data type with the specified precision and scale.
		[[nodiscard]] static DataType Decimal128(std::int32_t precision, std::int32_t scale); ///< @brief Creates a Decimal128 data type with the specified precision and scale.
		[[nodiscard]] static DataType Decimal256(std::int32_t precision, std::int32_t scale); ///< @brief Creates a Decimal256 data type with the specified precision and scale.
		[[nodiscard]] static DataType FixedBinary(std::size_t byte_count); ///< @brief Creates a FixedBinary data type with the specified byte count.
		[[nodiscard]] static DataType Time(TimeUnit unit); ///< @brief Creates a Time data type with the specified time unit.
		[[nodiscard]] static DataType Timestamp(TimeUnit unit, std::optional<std::string> time_zone = std::nullopt); ///< @brief Creates a Timestamp data type with the specified time unit and optional timezone.
		[[nodiscard]] static DataType Duration(TimeUnit unit); ///< @brief Creates a Duration data type with the specified time unit.
		[[nodiscard]] DataTypeKind getKind() const noexcept; ///< @brief Returns the kind of the data type.
		[[nodiscard]] const std::variant<std::monostate, DecimalSpec, FixedBinarySpec, TimeSpec, TimestampSpec, DurationSpec>& getParameters() const noexcept; ///< @brief Returns a borrowed, read-only reference to the parameters of the data type.

		[[nodiscard]] bool isSignedInteger() const noexcept; ///< @brief Checks if the data type is a signed integer.
		[[nodiscard]] bool isUnsignedInteger() const noexcept; ///< @brief Checks if the data type is an unsigned integer.
		[[nodiscard]] bool isInteger() const noexcept; ///< @brief Checks if the data type is an integer.
		[[nodiscard]] bool isFloatingPoint() const noexcept; ///< @brief Checks if the data type is a floating-point number.
		[[nodiscard]] bool isComplex() const noexcept; ///< @brief Checks if the data type is a complex number.
		[[nodiscard]] bool isDecimal() const noexcept; ///< @brief Checks if the data type is a decimal number.
		[[nodiscard]] bool isRealNumeric() const noexcept; ///< @brief Checks if the data type is an integer, floating-point, or decimal number.
		[[nodiscard]] bool isExactNumeric() const noexcept; ///< @brief Checks if the data type is an integer or decimal number.
		[[nodiscard]] bool isInexactNumeric() const noexcept; ///< @brief Checks if the data type is a floating-point or complex number.
		[[nodiscard]] bool isNumeric() const noexcept; ///< @brief Checks if the data type is an integer, floating-point, complex, or decimal number.
		[[nodiscard]] bool isString() const noexcept; ///< @brief Checks if the data type is a string.
		[[nodiscard]] bool isBinary() const noexcept; ///< @brief Checks if the data type is a variable-length or fixed-length binary type.
		[[nodiscard]] bool isVariableBinary() const noexcept; ///< @brief Checks if the data type is a variable-length binary type.
		[[nodiscard]] bool isFixedBinary() const noexcept; ///< @brief Checks if the data type is a fixed-length binary type.
		[[nodiscard]] bool isBoolean() const noexcept; ///< @brief Checks if the data type is a boolean type.
		[[nodiscard]] bool isNull() const noexcept; ///< @brief Checks if the data type is a null type.
		[[nodiscard]] bool isTemporal() const noexcept; ///< @brief Checks if the data type is a date, time, timestamp, or duration.
		[[nodiscard]] bool isDate() const noexcept; ///< @brief Checks if the data type is a date type.
		[[nodiscard]] bool isTime() const noexcept; ///< @brief Checks if the data type is a time type.
		[[nodiscard]] bool isTimestamp() const noexcept; ///< @brief Checks if the data type is a timestamp type.
		[[nodiscard]] bool isDuration() const noexcept; ///< @brief Checks if the data type is a duration type.

		[[nodiscard]] bool isInt8() const noexcept; ///< @brief Checks if the data type is an 8-bit signed integer type.
		[[nodiscard]] bool isInt16() const noexcept; ///< @brief Checks if the data type is a 16-bit signed integer type.
		[[nodiscard]] bool isInt32() const noexcept; ///< @brief Checks if the data type is a 32-bit signed integer type.
		[[nodiscard]] bool isInt64() const noexcept; ///< @brief Checks if the data type is a 64-bit signed integer type.
		[[nodiscard]] bool isUInt8() const noexcept; ///< @brief Checks if the data type is an unsigned 8-bit integer type.
		[[nodiscard]] bool isUInt16() const noexcept; ///< @brief Checks if the data type is an unsigned 16-bit integer type.
		[[nodiscard]] bool isUInt32() const noexcept; ///< @brief Checks if the data type is an unsigned 32-bit integer type.
		[[nodiscard]] bool isUInt64() const noexcept; ///< @brief Checks if the data type is an unsigned 64-bit integer type.
		[[nodiscard]] bool isFloat16() const noexcept; ///< @brief Checks if the data type is a 16-bit IEEE 754 floating-point type.
		[[nodiscard]] bool isBFloat16() const noexcept; ///< @brief Checks if the data type is a bfloat16 type.
		[[nodiscard]] bool isFloat32() const noexcept; ///< @brief Checks if the data type is a 32-bit IEEE 754 floating-point type.
		[[nodiscard]] bool isFloat64() const noexcept; ///< @brief Checks if the data type is a 64-bit IEEE 754 floating-point type.
		[[nodiscard]] bool isComplex64() const noexcept; ///< @brief Checks if the data type is a complex number with 32-bit components.
		[[nodiscard]] bool isComplex128() const noexcept; ///< @brief Checks if the data type is a complex number with 64-bit components.
		[[nodiscard]] bool isDecimal32() const noexcept; ///< @brief Checks if the data type is a Decimal32 type.
		[[nodiscard]] bool isDecimal64() const noexcept; ///< @brief Checks if the data type is a Decimal64 type.
		[[nodiscard]] bool isDecimal128() const noexcept; ///< @brief Checks if the data type is a Decimal128 type.
		[[nodiscard]] bool isDecimal256() const noexcept; ///< @brief Checks if the data type is a Decimal256 type.
		[[nodiscard]] bool hasParameters() const noexcept; ///< @brief Checks if the data type has associated parameters.
		[[nodiscard]] bool hasTimeUnit() const noexcept; ///< @brief Checks if the data type has an associated time unit. Recognizes TIME, TIMESTAMP, and DURATION types.
		[[nodiscard]] bool isTimezoneAware() const noexcept; ///< @brief Checks if the data type is a timezone-aware timestamp.
		[[nodiscard]] bool isTimezoneNaive() const noexcept; ///< @brief Checks if the data type is a timezone-naive timestamp.

		Create(kind parameters);
		TryCreate(kind, parameters);
		validateDefinition(kind, parameters);

		DataType(const DataType&) = default;
		DataType(DataType&&) noexcept = default;
		operator=(const DataType&) = default;
		operator=(DataType&&) noexcept = default;
		~DataType() noexcept = default;
		swap(DataType& other) noexcept;

		getName();
		visitParameters(visitor);

		getDecimalSpec();
		getFixedBinarySpec();
		getTimeSpec();
		getTimestampSpec();
		getDurationSpec();
		tryGetDecimalSpec();
		tryGetFixedBinarySpec();
		tryGetTimeSpec();
		tryGetTimestampSpec();
		tryGetDurationSpec();

		getPrecision();
		tryGetPrecision();
		getScale();	
		tryGetScale();
		getByteCount();
		tryGetByteCount();
		getTimeUnit();	
		tryGetTimeUnit();
		getTimezone();	
		tryGetTimezone();
		getMaxDecimalPrecision();	
		tryGetMaxDecimalPrecision();
		getComplexComponentType();	
		tryGetComplexComponentType();
		getNumericInfo();	
		tryGetNumericInfo();

		supportsNaN();
		supportsInfinity();
		supportsSignedZero();

		operator==(other);
		operator!=(other);
		operator<=>(other);
		operator()(const DataType&);
		operator<<(stream, type);
		swap(left, right);
		hasSameKind(other);
		hash();
		getFingerprint();

		withParameters(parameters);
		withPrecision(precision);
		withScale(scale);
		withDecimalSpec(spec);
		withByteCount(byte_count);
		withTimeUnit(unit);
		withTimezone(time_zone);
		withoutTimezone();

		toString();
		describe();
		Parse(text);
		TryParse(text);

		Of<T>();
		supportsCppType<T>();
		matchesCppType<T>();

		getCastInfo(target, options);
		canCastTo(target, options);
		canLosslesslyCastTo(target, options);
		promoteWith(other, options);
		CommonType(types, options);

		isFixedWidth(layout_policy);
		isVariableWidth(layout_policy);
		getBitWidth(layout_policy);
		getByteWidth(layout_policy);
		getLayout(layout_policy);
		hasSameLayout(other, layout_policy);

		serialize(writer);
		Deserialize(reader);
		toBytes();
		FromBytes(bytes);
		toJson();
		FromJson(text);

	private:
		DataType(DataTypeKind kind, std::variant<std::monostate, DecimalSpec, FixedBinarySpec, TimeSpec, TimestampSpec, DurationSpec> parameters);

		/**
		* @brief Checks whether a time unit is recognized.
		* @param unit The time unit to validate.
		* @throws std::invalid_argument If the unit is unrecognized.
		*/
		static void validateTimeUnit(TimeUnit unit);

		DataTypeKind kind_;
		std::variant<std::monostate, DecimalSpec, FixedBinarySpec, TimeSpec, TimestampSpec, DurationSpec> parameters_;
	};
}