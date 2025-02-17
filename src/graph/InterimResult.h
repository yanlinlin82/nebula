/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef GRAPH_INTERIMRESULT_H_
#define GRAPH_INTERIMRESULT_H_

#include "base/Base.h"
#include "base/StatusOr.h"
#include "filter/Expressions.h"
#include "dataman/RowSetReader.h"
#include "dataman/RowSetWriter.h"
#include "dataman/SchemaWriter.h"

namespace nebula {
namespace graph {
/**
 * The intermediate form of execution result, used in pipeline and variable.
 */
class InterimResult final {
public:
    InterimResult() = default;
    ~InterimResult() = default;
    InterimResult(const InterimResult &) = default;
    InterimResult& operator=(const InterimResult &) = default;
    InterimResult(InterimResult &&) = default;
    InterimResult& operator=(InterimResult &&) = default;

    explicit InterimResult(std::unique_ptr<RowSetWriter> rsWriter);
    explicit InterimResult(std::vector<VertexID> vids);

    static std::unique_ptr<InterimResult> getInterim(
            std::shared_ptr<const meta::SchemaProviderIf> resultSchema,
            std::vector<cpp2::RowValue> &rows);
    static Status castTo(cpp2::ColumnValue *col,
                         const nebula::cpp2::SupportedType &type);
    static Status castToInt(cpp2::ColumnValue *col);
    static Status castToDouble(cpp2::ColumnValue *col);
    static Status castToBool(cpp2::ColumnValue *col);
    static Status castToStr(cpp2::ColumnValue *col);

    std::shared_ptr<const meta::SchemaProviderIf> schema() const {
        return rsReader_->schema();
    }

    std::string& data() const {
        return rsWriter_->data();
    }

    StatusOr<std::vector<VertexID>> getVIDs(const std::string &col) const;

    StatusOr<std::vector<VertexID>> getDistinctVIDs(const std::string &col) const;

    std::vector<cpp2::RowValue> getRows() const;

    class InterimResultIndex;
    std::unique_ptr<InterimResultIndex> buildIndex(const std::string &vidColumn) const;

    class InterimResultIndex final {
    public:
        VariantType getColumnWithVID(VertexID id, const std::string &col) const;

    private:
        friend class InterimResult;
        using Row = std::vector<VariantType>;
        std::vector<Row>                            rows_;
        std::unordered_map<std::string, uint32_t>   columnToIndex_;
        std::unordered_map<VertexID, uint32_t>      vidToRowIndex_;
    };

private:
    using Row = std::vector<VariantType>;
    std::unique_ptr<RowSetReader>               rsReader_;
    std::unique_ptr<RowSetWriter>               rsWriter_;
    std::vector<VertexID>                       vids_;
};

}   // namespace graph
}   // namespace nebula

#endif  // GRAPH_INTERIMRESULT_H_
