#pragma once

#include <string>
#include <optional>

namespace ml
{
	/**
	*	@brief
	*	Identifies the semantic role of a field within a dataset.
	*/
	enum class FieldRole
	{
		FEATURE,	///< Input feature
		TARGET,		///< Expected output associated with a sample, including a classification label or regression value.
		ID,			///< Sample identifier. The required uniqueness scope should be defined and validated separately.
		WEIGHT,		///< Weight for the sample
		GROUP,		///< Group membership used for grouped splitting, evaluation, sampling, or ranking.
		TIMESTAMP,	///< Temporal information used for ordering, temporal splitting, or time-series processing.
		METADATA,	///< Auxiliary information retained alongside the dataset.
		IGNORE		///< A field excluded from default dataset processing.
	};

	/**
	* @brief
	* Describes a named column in a dataset, including its value type, semantic role, nullability, expected value shape, and validation rules.
	*/
	struct Field
	{
		std::string name; ///< Human-readable column name; schema rules govern uniqueness.
		DataType type; ///< Declared element type of the field’s values.
		FieldRole role = FieldRole::FEATURE; ///< Semantic role used by dataset operations; defaults to FEATURE.
		bool nullable = false; ///< Whether an entire field value may be missing; defaults to false.
		ShapeSpec shape; ///< Expected dimensions of one sample’s value, including any variable dimensions.
		std::optional<CategorySpec> category; ///< Optional category specification, including ordering and unknown-category handling.
		FieldConstraints constraints; ///< Additional validation rules applied to the field’s values.
		std::optional<FieldValue> default_value; ///< Optional declared default; must satisfy the type, shape, nullability, and constraints.
		Metadata metadata; ///< Descriptive annotations, such as units, descriptions, or provenance.
		FieldId field_id; ///< Stable column identifier preserved through renaming and reordering.
	};
}