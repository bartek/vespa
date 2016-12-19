// Copyright 2016 Yahoo Inc. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#pragma once

#include "attributevector.h"

namespace search {

struct NotImplementedAttribute : AttributeVector {
    using AttributeVector::AttributeVector;

    virtual void notImplemented() const ;

    uint32_t getValueCount(DocId) const override;
    largeint_t getInt(DocId) const override;
    double getFloat(DocId) const override;
    const char * getString(DocId, char *, size_t) const override;
    uint32_t get(DocId, largeint_t *, uint32_t) const override;
    uint32_t get(DocId, double *, uint32_t) const override;
    uint32_t get(DocId, vespalib::string *, uint32_t) const override;
    uint32_t get(DocId, const char **, uint32_t) const override;
    uint32_t get(DocId, EnumHandle *, uint32_t) const override;
    uint32_t get(DocId, WeightedInt *, uint32_t) const;
    uint32_t get(DocId, WeightedFloat *, uint32_t) const override;
    uint32_t get(DocId, WeightedString *, uint32_t) const override;
    uint32_t get(DocId, WeightedConstChar *, uint32_t) const override;
    uint32_t get(DocId, WeightedEnum *, uint32_t) const override;
    bool findEnum(const char *, EnumHandle &) const override;
    long onSerializeForAscendingSort(DocId, void *, long, const common::BlobConverter *) const override;
    long onSerializeForDescendingSort(DocId, void *, long, const common::BlobConverter *) const override;
    uint32_t clearDoc(DocId) override;
    int64_t getDefaultValue() const override;
    uint32_t getEnum(DocId) const override;
    void getEnumValue(const EnumHandle *, uint32_t *, uint32_t) const override;
    bool addDoc(DocId &) override;

    SearchContext::UP getSearch(QueryTermSimpleUP, const SearchContext::Params &) const override;
};

}  // namespace search

