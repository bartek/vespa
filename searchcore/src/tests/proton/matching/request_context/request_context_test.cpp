// Copyright 2019 Oath Inc. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include <vespa/eval/eval/tensor_spec.h>
#include <vespa/eval/tensor/serialization/typed_binary_format.h>
#include <vespa/eval/tensor/tensor.h>
#include <vespa/eval/tensor/test/test_utils.h>
#include <vespa/searchcore/proton/matching/requestcontext.h>
#include <vespa/searchlib/fef/properties.h>
#include <vespa/vespalib/gtest/gtest.h>
#include <vespa/vespalib/objects/nbostream.h>

using search::attribute::IAttributeContext;
using search::attribute::IAttributeFunctor;
using search::attribute::IAttributeVector;
using search::fef::Properties;
using vespalib::eval::TensorSpec;
using vespalib::tensor::Tensor;
using vespalib::tensor::TypedBinaryFormat;
using vespalib::tensor::test::makeTensor;
using namespace proton;

class MyAttributeContext : public search::attribute::IAttributeContext {
public:
    const IAttributeVector* getAttribute(const vespalib::string&) const override { abort(); }
    const IAttributeVector* getAttributeStableEnum(const vespalib::string&) const override { abort(); }
    void getAttributeList(std::vector<const IAttributeVector*>&) const override { abort(); }
    void asyncForAttribute(const vespalib::string&, std::unique_ptr<IAttributeFunctor>) const override { abort(); }
};

class RequestContextTest : public ::testing::Test {
private:
    vespalib::Clock _clock;
    vespalib::Doom _doom;
    MyAttributeContext _attr_ctx;
    Properties _props;
    RequestContext _request_ctx;
    Tensor::UP _query_tensor;

    void insert_tensor_in_properties(const vespalib::string& tensor_name, const Tensor& tensor) {
        vespalib::nbostream stream;
        TypedBinaryFormat::serialize(stream, tensor);
        _props.add(tensor_name, vespalib::stringref(stream.c_str(), stream.size()));
    }

public:
    RequestContextTest()
        : _clock(),
          _doom(_clock, fastos::SteadyTimeStamp()),
          _attr_ctx(),
          _props(),
          _request_ctx(_doom, _attr_ctx, _props),
          _query_tensor(makeTensor<Tensor>(TensorSpec("tensor(x[2])")
                                                   .add({{"x", 0}}, 3).add({{"x", 1}}, 5)))
    {
        insert_tensor_in_properties("my_tensor", *_query_tensor);
        _props.add("my_string", "foo bar");
    }
    const Tensor& expected_query_tensor() const { return *_query_tensor; }
    Tensor::UP get_query_tensor(const vespalib::string& tensor_name) const {
        return _request_ctx.get_query_tensor(tensor_name);
    }
};

TEST_F(RequestContextTest, query_tensor_can_be_retrieved)
{
    auto tensor = get_query_tensor("my_tensor");
    ASSERT_TRUE(tensor);
    EXPECT_EQUAL(expected_query_tensor(), *tensor);
}

TEST_F(RequestContextTest, non_existing_query_tensor_returns_nullptr)
{
    auto tensor = get_query_tensor("non_existing");
    EXPECT_FALSE(tensor);
}

TEST_F(RequestContextTest, rank_property_of_non_tensor_type_returns_nullptr)
{
    auto tensor = get_query_tensor("my_string");
    EXPECT_FALSE(tensor);
}

GTEST_MAIN_RUN_ALL_TESTS()
