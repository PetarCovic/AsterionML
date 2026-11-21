#include <ml/data/DataType.hpp>
#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace
{
    using ml::DataType;
    using ml::DataTypeKind;
    using ml::TimeUnit;

    using Factory = DataType(*)();
    using Predicate = bool (DataType::*)() const noexcept;
    using DecimalFactory = DataType(*)(std::int32_t, std::int32_t);

    /**
     * @brief Flags describing the expected classification families of a kind.
     */
    enum Family : std::uint32_t
    {
        SIGNED_INTEGER = 1u << 0, ///< Signed integer.
        UNSIGNED_INTEGER = 1u << 1, ///< Unsigned integer.
        INTEGER = 1u << 2, ///< Signed or unsigned integer.
        FLOATING_POINT = 1u << 3, ///< Binary floating-point, including bfloat16.
        COMPLEX = 1u << 4, ///< Complex number.
        DECIMAL = 1u << 5, ///< Fixed-point decimal.
        REAL_NUMERIC = 1u << 6, ///< Integer, floating-point, or decimal.
        EXACT_NUMERIC = 1u << 7, ///< Integer or decimal.
        INEXACT_NUMERIC = 1u << 8, ///< Floating-point or complex.
        NUMERIC = 1u << 9, ///< Any supported numeric kind.
        BINARY = 1u << 10, ///< Variable-length or fixed-length binary.
        TEMPORAL = 1u << 11 ///< Date, time, timestamp, or duration.
    };

    constexpr std::uint32_t signedFamilies =
        SIGNED_INTEGER | INTEGER | REAL_NUMERIC | EXACT_NUMERIC | NUMERIC;

    constexpr std::uint32_t unsignedFamilies =
        UNSIGNED_INTEGER | INTEGER | REAL_NUMERIC | EXACT_NUMERIC | NUMERIC;

    constexpr std::uint32_t floatingFamilies =
        FLOATING_POINT | REAL_NUMERIC | INEXACT_NUMERIC | NUMERIC;

    constexpr std::uint32_t complexFamilies =
        COMPLEX | INEXACT_NUMERIC | NUMERIC;

    constexpr std::uint32_t decimalFamilies =
        DECIMAL | REAL_NUMERIC | EXACT_NUMERIC | NUMERIC;

    /**
     * @brief Expected timestamp zone state.
     */
    enum class ZoneState
    {
        NOT_TIMESTAMP, ///< Neither timestamp zone query matches.
        NAIVE, ///< Timestamp without a declared zone.
        AWARE ///< Timestamp with a declared zone.
    };

    /**
     * @brief A public factory with independently declared expected properties.
     */
    struct KindCase
    {
        std::string_view name; ///< Readable, alphanumeric case name.
        Factory factory; ///< Factory invoked inside the test.
        DataTypeKind kind; ///< Expected logical kind.
        std::uint32_t families; ///< Expected family flags.
        bool parameters = false; ///< Expected parameter presence.
        bool time_unit = false; ///< Expected time-unit presence.
        ZoneState zone = ZoneState::NOT_TIMESTAMP; ///< Expected zone state.
    };

    /**
     * @brief Covers all kinds and both timestamp zone states.
     */
    [[nodiscard]] std::array<KindCase, 28> makeKindCases()
    {
        return { {
            { "Int8", &DataType::Int8, DataTypeKind::INT8, signedFamilies },
            { "Int16", &DataType::Int16, DataTypeKind::INT16, signedFamilies },
            { "Int32", &DataType::Int32, DataTypeKind::INT32, signedFamilies },
            { "Int64", &DataType::Int64, DataTypeKind::INT64, signedFamilies },
            { "UInt8", &DataType::UInt8, DataTypeKind::UINT8, unsignedFamilies },
            { "UInt16", &DataType::UInt16, DataTypeKind::UINT16, unsignedFamilies },
            { "UInt32", &DataType::UInt32, DataTypeKind::UINT32, unsignedFamilies },
            { "UInt64", &DataType::UInt64, DataTypeKind::UINT64, unsignedFamilies },
            { "Float16", &DataType::Float16, DataTypeKind::FLOAT16, floatingFamilies },
            { "BFloat16", &DataType::BFloat16, DataTypeKind::BFLOAT16, floatingFamilies },
            { "Float32", &DataType::Float32, DataTypeKind::FLOAT32, floatingFamilies },
            { "Float64", &DataType::Float64, DataTypeKind::FLOAT64, floatingFamilies },
            { "Complex64", &DataType::Complex64, DataTypeKind::COMPLEX64, complexFamilies },
            { "Complex128", &DataType::Complex128, DataTypeKind::COMPLEX128, complexFamilies },
            {
                "Decimal32",
                []() { return DataType::Decimal32(9, 2); },
                DataTypeKind::DECIMAL32, decimalFamilies, true
            },
            {
                "Decimal64",
                []() { return DataType::Decimal64(18, 2); },
                DataTypeKind::DECIMAL64, decimalFamilies, true
            },
            {
                "Decimal128",
                []() { return DataType::Decimal128(38, 2); },
                DataTypeKind::DECIMAL128, decimalFamilies, true
            },
            {
                "Decimal256",
                []() { return DataType::Decimal256(76, 2); },
                DataTypeKind::DECIMAL256, decimalFamilies, true
            },
            { "String", &DataType::String, DataTypeKind::STRING, 0 },
            { "Binary", &DataType::Binary, DataTypeKind::BINARY, BINARY },
            {
                "FixedBinary",
                []() { return DataType::FixedBinary(16); },
                DataTypeKind::FIXED_BINARY, BINARY, true
            },
            { "Boolean", &DataType::Boolean, DataTypeKind::BOOLEAN, 0 },
            { "Date", &DataType::Date, DataTypeKind::DATE, TEMPORAL },
            {
                "Time",
                []() { return DataType::Time(TimeUnit::NANOSECOND); },
                DataTypeKind::TIME, TEMPORAL, true, true
            },
            {
                "TimestampNaive",
                []() { return DataType::Timestamp(TimeUnit::NANOSECOND); },
                DataTypeKind::TIMESTAMP, TEMPORAL, true, true, ZoneState::NAIVE
            },
            {
                "TimestampUtc",
                []() { return DataType::Timestamp(TimeUnit::NANOSECOND, std::string{ "UTC" }); },
                DataTypeKind::TIMESTAMP, TEMPORAL, true, true, ZoneState::AWARE
            },
            {
                "Duration",
                []() { return DataType::Duration(TimeUnit::NANOSECOND); },
                DataTypeKind::DURATION, TEMPORAL, true, true
            },
            { "Null", &DataType::Null, DataTypeKind::NULL_VALUE, 0 }
        } };
    }

    /**
     * @brief An exact-kind predicate and its sole matching kind.
     */
    struct ExactQuery
    {
        std::string_view name; ///< Alphanumeric predicate label.
        DataTypeKind kind; ///< Sole matching kind.
        Predicate query; ///< Public query under test.
    };

    constexpr std::array<ExactQuery, 27> exactQueries{ {
        { "IsInt8", DataTypeKind::INT8, &DataType::isInt8 },
        { "IsInt16", DataTypeKind::INT16, &DataType::isInt16 },
        { "IsInt32", DataTypeKind::INT32, &DataType::isInt32 },
        { "IsInt64", DataTypeKind::INT64, &DataType::isInt64 },
        { "IsUInt8", DataTypeKind::UINT8, &DataType::isUInt8 },
        { "IsUInt16", DataTypeKind::UINT16, &DataType::isUInt16 },
        { "IsUInt32", DataTypeKind::UINT32, &DataType::isUInt32 },
        { "IsUInt64", DataTypeKind::UINT64, &DataType::isUInt64 },
        { "IsFloat16", DataTypeKind::FLOAT16, &DataType::isFloat16 },
        { "IsBFloat16", DataTypeKind::BFLOAT16, &DataType::isBFloat16 },
        { "IsFloat32", DataTypeKind::FLOAT32, &DataType::isFloat32 },
        { "IsFloat64", DataTypeKind::FLOAT64, &DataType::isFloat64 },
        { "IsComplex64", DataTypeKind::COMPLEX64, &DataType::isComplex64 },
        { "IsComplex128", DataTypeKind::COMPLEX128, &DataType::isComplex128 },
        { "IsDecimal32", DataTypeKind::DECIMAL32, &DataType::isDecimal32 },
        { "IsDecimal64", DataTypeKind::DECIMAL64, &DataType::isDecimal64 },
        { "IsDecimal128", DataTypeKind::DECIMAL128, &DataType::isDecimal128 },
        { "IsDecimal256", DataTypeKind::DECIMAL256, &DataType::isDecimal256 },
        { "IsString", DataTypeKind::STRING, &DataType::isString },
        { "IsVariableBinary", DataTypeKind::BINARY, &DataType::isVariableBinary },
        { "IsFixedBinary", DataTypeKind::FIXED_BINARY, &DataType::isFixedBinary },
        { "IsBoolean", DataTypeKind::BOOLEAN, &DataType::isBoolean },
        { "IsDate", DataTypeKind::DATE, &DataType::isDate },
        { "IsTime", DataTypeKind::TIME, &DataType::isTime },
        { "IsTimestamp", DataTypeKind::TIMESTAMP, &DataType::isTimestamp },
        { "IsDuration", DataTypeKind::DURATION, &DataType::isDuration },
        { "IsNull", DataTypeKind::NULL_VALUE, &DataType::isNull }
    } };

    /**
     * @brief A broad-family query and its expected membership flag.
     */
    struct FamilyQuery
    {
        std::string_view name; ///< Alphanumeric predicate label.
        Predicate query; ///< Public query under test.
        std::uint32_t family; ///< Expected membership flag.
    };

    constexpr std::array<FamilyQuery, 12> familyQueries{ {
        { "IsSignedInteger", &DataType::isSignedInteger, SIGNED_INTEGER },
        { "IsUnsignedInteger", &DataType::isUnsignedInteger, UNSIGNED_INTEGER },
        { "IsInteger", &DataType::isInteger, INTEGER },
        { "IsFloatingPoint", &DataType::isFloatingPoint, FLOATING_POINT },
        { "IsComplex", &DataType::isComplex, COMPLEX },
        { "IsDecimal", &DataType::isDecimal, DECIMAL },
        { "IsRealNumeric", &DataType::isRealNumeric, REAL_NUMERIC },
        { "IsExactNumeric", &DataType::isExactNumeric, EXACT_NUMERIC },
        { "IsInexactNumeric", &DataType::isInexactNumeric, INEXACT_NUMERIC },
        { "IsNumeric", &DataType::isNumeric, NUMERIC },
        { "IsBinary", &DataType::isBinary, BINARY },
        { "IsTemporal", &DataType::isTemporal, TEMPORAL }
    } };

    /**
     * @brief One independently reported kind-and-predicate combination.
     */
    struct PredicateCase
    {
        std::string name; ///< Unique alphanumeric case name.
        Factory factory; ///< Factory invoked inside the test.
        Predicate query; ///< Predicate under test.
        bool expected; ///< Expected query result.
    };

    /**
     * @brief Generates all 43 predicates for every representative descriptor.
     */
    [[nodiscard]] std::vector<PredicateCase> makePredicateCases()
    {
        const auto kinds = makeKindCases();
        std::vector<PredicateCase> cases;
        cases.reserve(kinds.size() * (exactQueries.size() + familyQueries.size() + 4));

        for (const auto& kind : kinds)
        {
            const auto addCase = [&](std::string_view name, Predicate query, bool expected)
                {
                    cases.push_back({
                        std::string{ kind.name } + std::string{ name },
                        kind.factory,
                        query,
                        expected
                        });
                };

            for (const auto& entry : exactQueries)
            {
                addCase(entry.name, entry.query, kind.kind == entry.kind);
            }

            for (const auto& entry : familyQueries)
            {
                addCase(entry.name, entry.query, (kind.families & entry.family) != 0);
            }

            addCase("HasParameters", &DataType::hasParameters, kind.parameters);
            addCase("HasTimeUnit", &DataType::hasTimeUnit, kind.time_unit);
            addCase("IsTimezoneAware", &DataType::isTimezoneAware, kind.zone == ZoneState::AWARE);
            addCase("IsTimezoneNaive", &DataType::isTimezoneNaive, kind.zone == ZoneState::NAIVE);
        }

        return cases;
    }

    /**
     * @brief A decimal factory and its precision limit.
     */
    struct DecimalKind
    {
        std::string_view name; ///< Decimal kind label.
        DataTypeKind kind; ///< Expected logical kind.
        std::int32_t max_precision; ///< Maximum supported precision.
        DecimalFactory factory; ///< Public factory under test.
    };

    constexpr std::array<DecimalKind, 4> decimalKinds{ {
        { "Decimal32", DataTypeKind::DECIMAL32, 9, &DataType::Decimal32 },
        { "Decimal64", DataTypeKind::DECIMAL64, 18, &DataType::Decimal64 },
        { "Decimal128", DataTypeKind::DECIMAL128, 38, &DataType::Decimal128 },
        { "Decimal256", DataTypeKind::DECIMAL256, 76, &DataType::Decimal256 }
    } };

    /**
     * @brief Named arguments for one decimal construction case.
     */
    struct DecimalCase
    {
        std::string name; ///< Unique alphanumeric case name.
        DecimalFactory factory; ///< Public factory under test.
        DataTypeKind kind; ///< Expected logical kind.
        std::int32_t precision; ///< Precision argument.
        std::int32_t scale; ///< Scale argument.
    };

    /**
     * @brief A readable label for an integer boundary argument.
     */
    struct NamedInteger
    {
        std::string_view name; ///< Alphanumeric boundary label.
        std::int32_t value; ///< Boundary argument.
    };

    /**
     * @brief Generates valid decimal precision and signed-scale combinations.
     */
    [[nodiscard]] std::vector<DecimalCase> makeValidDecimalCases()
    {
        std::vector<DecimalCase> cases;

        for (const auto& entry : decimalKinds)
        {
            const std::array<std::int32_t, 3> precisions{
                1, entry.max_precision / 2, entry.max_precision
            };

            const std::array<NamedInteger, 6> scales{ {
                { "MinScale", std::numeric_limits<std::int32_t>::min() },
                { "NegativeTwoScale", -2 },
                { "ZeroScale", 0 },
                { "TwoScale", 2 },
                { "AbovePrecisionScale", entry.max_precision + 1 },
                { "MaxScale", std::numeric_limits<std::int32_t>::max() }
            } };

            for (const auto precision : precisions)
            {
                for (const auto& scale : scales)
                {
                    cases.push_back({
                        std::string{ entry.name } + "Precision"
                            + std::to_string(precision) + std::string{ scale.name },
                        entry.factory, entry.kind, precision, scale.value
                        });
                }
            }
        }

        return cases;
    }

    /**
     * @brief Generates invalid precision arguments for all decimal factories.
     */
    [[nodiscard]] std::vector<DecimalCase> makeInvalidDecimalCases()
    {
        std::vector<DecimalCase> cases;

        for (const auto& entry : decimalKinds)
        {
            const std::array<NamedInteger, 5> precisions{ {
                { "MinPrecision", std::numeric_limits<std::int32_t>::min() },
                { "NegativePrecision", -1 },
                { "ZeroPrecision", 0 },
                { "AboveMaxPrecision", entry.max_precision + 1 },
                { "MaxIntPrecision", std::numeric_limits<std::int32_t>::max() }
            } };

            for (const auto& precision : precisions)
            {
                cases.push_back({
                    std::string{ entry.name } + std::string{ precision.name },
                    entry.factory, entry.kind, precision.value, 0
                    });
            }
        }

        return cases;
    }

    /**
     * @brief Named fixed-binary length argument.
     */
    struct FixedBinaryCase
    {
        std::string_view name; ///< Alphanumeric case name.
        std::size_t byte_count; ///< Declared length; no payload is allocated.
    };

    constexpr std::array<FixedBinaryCase, 4> fixedBinaryCases{ {
        { "ZeroBytes", 0 },
        { "OneByte", 1 },
        { "SixteenBytes", 16 },
        { "MaximumBytes", std::numeric_limits<std::size_t>::max() }
    } };

    /**
     * @brief Named temporal-unit argument.
     */
    struct TimeUnitCase
    {
        std::string_view name; ///< Alphanumeric case name.
        TimeUnit unit; ///< Unit argument.
    };

    constexpr std::array<TimeUnitCase, 6> timeUnitCases{ {
        { "Second", TimeUnit::SECOND },
        { "Millisecond", TimeUnit::MILLISECOND },
        { "Microsecond", TimeUnit::MICROSECOND },
        { "Nanosecond", TimeUnit::NANOSECOND },
        { "Picosecond", TimeUnit::PICOSECOND },
        { "Femtosecond", TimeUnit::FEMTOSECOND }
    } };

    constexpr std::array<TimeUnitCase, 2> invalidTimeUnitCases{ {
        { "NegativeUnit", static_cast<TimeUnit>(-1) },
        { "UnknownUnit", static_cast<TimeUnit>(999) }
    } };

    /**
     * @brief Named timestamp arguments with an explicitly supplied optional zone.
     */
    struct TimestampCase
    {
        std::string name; ///< Unique alphanumeric case name.
        TimeUnit unit; ///< Declared time unit.
        std::optional<std::string> time_zone; ///< Supplied zone argument.
    };

    /**
     * @brief Generates explicit absent and declared zones for all supported units.
     */
    [[nodiscard]] std::vector<TimestampCase> makeTimestampCases()
    {
        std::vector<TimestampCase> cases;

        for (const auto& entry : timeUnitCases)
        {
            const auto prefix = std::string{ entry.name };

            cases.push_back({ prefix + "Absent", entry.unit, std::nullopt });
            cases.push_back({ prefix + "Utc", entry.unit, std::string{ "UTC" } });
            cases.push_back({ prefix + "NewYork", entry.unit, std::string{ "America/New_York" } });
            cases.push_back({
                prefix + "BuenosAires",
                entry.unit,
                std::string{ "America/Argentina/Buenos_Aires" }
                });
        }

        return cases;
    }

    /** @brief Supplies a readable kind-case description to GoogleTest. */
    void PrintTo(const KindCase& value, std::ostream* output)
    {
        *output << value.name;
    }

    /** @brief Supplies a readable predicate-case description to GoogleTest. */
    void PrintTo(const PredicateCase& value, std::ostream* output)
    {
        *output << value.name;
    }

    /** @brief Supplies a readable decimal-case description to GoogleTest. */
    void PrintTo(const DecimalCase& value, std::ostream* output)
    {
        *output << value.name;
    }

    /** @brief Supplies a readable binary-case description to GoogleTest. */
    void PrintTo(const FixedBinaryCase& value, std::ostream* output)
    {
        *output << value.name;
    }

    /** @brief Supplies a readable temporal-unit description to GoogleTest. */
    void PrintTo(const TimeUnitCase& value, std::ostream* output)
    {
        *output << value.name;
    }

    /** @brief Supplies a readable timestamp-case description to GoogleTest. */
    void PrintTo(const TimestampCase& value, std::ostream* output)
    {
        *output << value.name;
    }

    /**
     * @brief Uses the case's alphanumeric name as its parameter suffix.
     */
    template<typename Case>
    [[nodiscard]] std::string parameterName(const ::testing::TestParamInfo<Case>& info)
    {
        return std::string{ info.param.name };
    }

    /** @brief Checks construction through every named type factory. */
    class DataTypeKindTest : public ::testing::TestWithParam<KindCase>
    {
    };

    TEST_P(DataTypeKindTest, PreservesKindAndParameterStorage)
    {
        const auto& entry = GetParam();
        const auto type = entry.factory();

        EXPECT_EQ(type.getKind(), entry.kind);
        EXPECT_EQ(
            !std::holds_alternative<std::monostate>(type.getParameters()),
            entry.parameters);
    }

    INSTANTIATE_TEST_SUITE_P(
        AllKinds, DataTypeKindTest,
        ::testing::ValuesIn(makeKindCases()), parameterName<KindCase>);

    /** @brief Reports each kind-and-predicate combination independently. */
    class DataTypePredicateTest : public ::testing::TestWithParam<PredicateCase>
    {
    };

    TEST_P(DataTypePredicateTest, MatchesExpectedResult)
    {
        const auto& entry = GetParam();
        const auto type = entry.factory();

        EXPECT_EQ((type.*entry.query)(), entry.expected);
    }

    INSTANTIATE_TEST_SUITE_P(
        Classification, DataTypePredicateTest,
        ::testing::ValuesIn(makePredicateCases()), parameterName<PredicateCase>);

    /** @brief Checks valid decimal construction and parameter preservation. */
    class DataTypeDecimalTest : public ::testing::TestWithParam<DecimalCase>
    {
    };

    TEST_P(DataTypeDecimalTest, PreservesPrecisionAndScale)
    {
        const auto& entry = GetParam();
        const auto type = entry.factory(entry.precision, entry.scale);

        EXPECT_EQ(type.getKind(), entry.kind);

        const auto* spec = std::get_if<ml::DecimalSpec>(&type.getParameters());
        ASSERT_NE(spec, nullptr);

        EXPECT_EQ(spec->precision, entry.precision);
        EXPECT_EQ(spec->scale, entry.scale);
        EXPECT_TRUE(type.isDecimal());
        EXPECT_TRUE(type.isExactNumeric());
        EXPECT_TRUE(type.isNumeric());
        EXPECT_TRUE(type.hasParameters());
    }

    INSTANTIATE_TEST_SUITE_P(
        ValidBoundaries, DataTypeDecimalTest,
        ::testing::ValuesIn(makeValidDecimalCases()), parameterName<DecimalCase>);

    /** @brief Checks rejection of invalid decimal precision arguments. */
    class DataTypeInvalidDecimalTest : public ::testing::TestWithParam<DecimalCase>
    {
    };

    TEST_P(DataTypeInvalidDecimalTest, RejectsInvalidPrecision)
    {
        const auto& entry = GetParam();

        EXPECT_THROW(
            static_cast<void>(entry.factory(entry.precision, entry.scale)),
            std::invalid_argument);
    }

    INSTANTIATE_TEST_SUITE_P(
        InvalidBoundaries, DataTypeInvalidDecimalTest,
        ::testing::ValuesIn(makeInvalidDecimalCases()), parameterName<DecimalCase>);

    /** @brief Checks fixed-binary descriptors without allocating binary payloads. */
    class DataTypeFixedBinaryTest : public ::testing::TestWithParam<FixedBinaryCase>
    {
    };

    TEST_P(DataTypeFixedBinaryTest, PreservesDeclaredLength)
    {
        const auto& entry = GetParam();
        const auto type = DataType::FixedBinary(entry.byte_count);

        EXPECT_EQ(type.getKind(), DataTypeKind::FIXED_BINARY);

        const auto* spec = std::get_if<ml::FixedBinarySpec>(&type.getParameters());
        ASSERT_NE(spec, nullptr);

        EXPECT_EQ(spec->byte_count, entry.byte_count);
        EXPECT_TRUE(type.isBinary());
        EXPECT_TRUE(type.isFixedBinary());
        EXPECT_FALSE(type.isVariableBinary());
        EXPECT_TRUE(type.hasParameters());
    }

    INSTANTIATE_TEST_SUITE_P(
        LengthBoundaries, DataTypeFixedBinaryTest,
        ::testing::ValuesIn(fixedBinaryCases), parameterName<FixedBinaryCase>);

    /** @brief Checks temporal construction for every supported unit. */
    class DataTypeTemporalTest : public ::testing::TestWithParam<TimeUnitCase>
    {
    };

    TEST_P(DataTypeTemporalTest, TimePreservesUnit)
    {
        const auto& entry = GetParam();
        const auto type = DataType::Time(entry.unit);

        EXPECT_EQ(type.getKind(), DataTypeKind::TIME);

        const auto* spec = std::get_if<ml::TimeSpec>(&type.getParameters());
        ASSERT_NE(spec, nullptr);

        EXPECT_EQ(spec->unit, entry.unit);
        EXPECT_TRUE(type.isTemporal());
        EXPECT_TRUE(type.hasTimeUnit());
        EXPECT_TRUE(type.hasParameters());
    }

    TEST_P(DataTypeTemporalTest, TimestampDefaultsToUnspecifiedZone)
    {
        const auto& entry = GetParam();
        const auto type = DataType::Timestamp(entry.unit);

        EXPECT_EQ(type.getKind(), DataTypeKind::TIMESTAMP);

        const auto* spec = std::get_if<ml::TimestampSpec>(&type.getParameters());
        ASSERT_NE(spec, nullptr);

        EXPECT_EQ(spec->unit, entry.unit);
        EXPECT_FALSE(spec->time_zone.has_value());
        EXPECT_TRUE(type.isTimezoneNaive());
        EXPECT_FALSE(type.isTimezoneAware());
        EXPECT_TRUE(type.hasTimeUnit());
    }

    TEST_P(DataTypeTemporalTest, DurationPreservesUnit)
    {
        const auto& entry = GetParam();
        const auto type = DataType::Duration(entry.unit);

        EXPECT_EQ(type.getKind(), DataTypeKind::DURATION);

        const auto* spec = std::get_if<ml::DurationSpec>(&type.getParameters());
        ASSERT_NE(spec, nullptr);

        EXPECT_EQ(spec->unit, entry.unit);
        EXPECT_TRUE(type.isTemporal());
        EXPECT_TRUE(type.hasTimeUnit());
        EXPECT_TRUE(type.hasParameters());
    }

    TEST_P(DataTypeTemporalTest, RejectsExplicitlyEmptyTimestampZone)
    {
        EXPECT_THROW(
            static_cast<void>(DataType::Timestamp(GetParam().unit, std::string{})),
            std::invalid_argument);
    }

    INSTANTIATE_TEST_SUITE_P(
        SupportedUnits, DataTypeTemporalTest,
        ::testing::ValuesIn(timeUnitCases), parameterName<TimeUnitCase>);

    /** @brief Checks rejection of unrecognized temporal-unit enum values. */
    class DataTypeInvalidTimeUnitTest : public ::testing::TestWithParam<TimeUnitCase>
    {
    };

    TEST_P(DataTypeInvalidTimeUnitTest, TimeRejectsInvalidUnit)
    {
        EXPECT_THROW(
            static_cast<void>(DataType::Time(GetParam().unit)),
            std::invalid_argument);
    }

    TEST_P(DataTypeInvalidTimeUnitTest, NaiveTimestampRejectsInvalidUnit)
    {
        EXPECT_THROW(
            static_cast<void>(DataType::Timestamp(GetParam().unit)),
            std::invalid_argument);
    }

    TEST_P(DataTypeInvalidTimeUnitTest, AwareTimestampRejectsInvalidUnit)
    {
        EXPECT_THROW(
            static_cast<void>(DataType::Timestamp(GetParam().unit, std::string{ "UTC" })),
            std::invalid_argument);
    }

    TEST_P(DataTypeInvalidTimeUnitTest, DurationRejectsInvalidUnit)
    {
        EXPECT_THROW(
            static_cast<void>(DataType::Duration(GetParam().unit)),
            std::invalid_argument);
    }

    INSTANTIATE_TEST_SUITE_P(
        InvalidUnits, DataTypeInvalidTimeUnitTest,
        ::testing::ValuesIn(invalidTimeUnitCases), parameterName<TimeUnitCase>);

    /** @brief Checks explicitly absent and declared timestamp time zones. */
    class DataTypeTimestampTest : public ::testing::TestWithParam<TimestampCase>
    {
    };

    TEST_P(DataTypeTimestampTest, PreservesUnitZoneAndAwareness)
    {
        const auto& entry = GetParam();
        const auto type = DataType::Timestamp(entry.unit, entry.time_zone);

        EXPECT_EQ(type.getKind(), DataTypeKind::TIMESTAMP);

        const auto* spec = std::get_if<ml::TimestampSpec>(&type.getParameters());
        ASSERT_NE(spec, nullptr);

        EXPECT_EQ(spec->unit, entry.unit);
        EXPECT_EQ(spec->time_zone, entry.time_zone);
        EXPECT_EQ(type.isTimezoneAware(), entry.time_zone.has_value());
        EXPECT_EQ(type.isTimezoneNaive(), !entry.time_zone.has_value());
        EXPECT_TRUE(type.hasTimeUnit());
        EXPECT_TRUE(type.hasParameters());
    }

    INSTANTIATE_TEST_SUITE_P(
        TimezoneArguments, DataTypeTimestampTest,
        ::testing::ValuesIn(makeTimestampCases()), parameterName<TimestampCase>);
}