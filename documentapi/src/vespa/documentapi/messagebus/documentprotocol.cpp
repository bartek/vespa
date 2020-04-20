// Copyright 2017 Yahoo Holdings. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include "routablefactories60.h"
#include "routingpolicyfactories.h"
#include "routablerepository.h"
#include "routingpolicyrepository.h"
#include "replymerger.h"
#include <vespa/document/util/stringutil.h>
#include <vespa/documentapi/documentapi.h>
#include <vespa/messagebus/emptyreply.h>
#include <vespa/vespalib/util/exceptions.h>
#include <sstream>

#include <vespa/log/log.h>
LOG_SETUP(".documentprotocol");

using document::DocumentTypeRepo;

namespace documentapi {

const mbus::string DocumentProtocol::NAME = "document";

DocumentProtocol::DocumentProtocol(const LoadTypeSet& loadTypes,
                                   std::shared_ptr<const DocumentTypeRepo> repo,
                                   const string &configId) :
    _routingPolicyRepository(std::make_unique<RoutingPolicyRepository>()),
    _routableRepository(std::make_unique<RoutableRepository>(loadTypes)),
    _repo(std::move(repo))
{
    // Prepare config string for routing policy factories.
    string cfg = (configId.empty() ? "client" : configId);

    // When adding factories to this list, please KEEP THEM ORDERED alphabetically like they are now.
    putRoutingPolicyFactory("AND", IRoutingPolicyFactory::SP(new RoutingPolicyFactories::AndPolicyFactory()));
    putRoutingPolicyFactory("Content", IRoutingPolicyFactory::SP(new RoutingPolicyFactories::ContentPolicyFactory()));
    putRoutingPolicyFactory("MessageType", IRoutingPolicyFactory::SP(new RoutingPolicyFactories::MessageTypePolicyFactory()));
    putRoutingPolicyFactory("DocumentRouteSelector", IRoutingPolicyFactory::SP(new RoutingPolicyFactories::DocumentRouteSelectorPolicyFactory(*_repo, cfg)));
    putRoutingPolicyFactory("Extern", IRoutingPolicyFactory::SP(new RoutingPolicyFactories::ExternPolicyFactory()));
    putRoutingPolicyFactory("LocalService", IRoutingPolicyFactory::SP(new RoutingPolicyFactories::LocalServicePolicyFactory()));
    putRoutingPolicyFactory("RoundRobin", IRoutingPolicyFactory::SP(new RoutingPolicyFactories::RoundRobinPolicyFactory()));
    putRoutingPolicyFactory("Storage", IRoutingPolicyFactory::SP(new RoutingPolicyFactories::StoragePolicyFactory()));
    putRoutingPolicyFactory("SubsetService", IRoutingPolicyFactory::SP(new RoutingPolicyFactories::SubsetServicePolicyFactory()));
    putRoutingPolicyFactory("LoadBalancer", IRoutingPolicyFactory::SP(new RoutingPolicyFactories::LoadBalancerPolicyFactory()));

    // Prepare version specifications to use when adding routable factories.
    vespalib::VersionSpecification version6(6, 221);

    std::vector<vespalib::VersionSpecification> from6  = { version6 };

    // Add 6.x serialization
    putRoutableFactory(MESSAGE_CREATEVISITOR, IRoutableFactory::SP(new RoutableFactories60::CreateVisitorMessageFactory()), from6);
    putRoutableFactory(MESSAGE_DESTROYVISITOR, IRoutableFactory::SP(new RoutableFactories60::DestroyVisitorMessageFactory()), from6);
    putRoutableFactory(MESSAGE_DOCUMENTLIST, IRoutableFactory::SP(new RoutableFactories60::DocumentListMessageFactory(*_repo)), from6);
    putRoutableFactory(MESSAGE_DOCUMENTSUMMARY, IRoutableFactory::SP(new RoutableFactories60::DocumentSummaryMessageFactory()), from6);
    putRoutableFactory(MESSAGE_EMPTYBUCKETS, IRoutableFactory::SP(new RoutableFactories60::EmptyBucketsMessageFactory()), from6);
    putRoutableFactory(MESSAGE_GETBUCKETLIST, IRoutableFactory::SP(new RoutableFactories60::GetBucketListMessageFactory()), from6);
    putRoutableFactory(MESSAGE_GETBUCKETSTATE, IRoutableFactory::SP(new RoutableFactories60::GetBucketStateMessageFactory()), from6);
    putRoutableFactory(MESSAGE_GETDOCUMENT, IRoutableFactory::SP(new RoutableFactories60::GetDocumentMessageFactory()), from6);
    putRoutableFactory(MESSAGE_MAPVISITOR, IRoutableFactory::SP(new RoutableFactories60::MapVisitorMessageFactory()), from6);
    putRoutableFactory(MESSAGE_PUTDOCUMENT, IRoutableFactory::SP(new RoutableFactories60::PutDocumentMessageFactory(*_repo)), from6);
    putRoutableFactory(MESSAGE_QUERYRESULT, IRoutableFactory::SP(new RoutableFactories60::QueryResultMessageFactory()), from6);
    putRoutableFactory(MESSAGE_REMOVEDOCUMENT, IRoutableFactory::SP(new RoutableFactories60::RemoveDocumentMessageFactory()), from6);
    putRoutableFactory(MESSAGE_REMOVELOCATION, IRoutableFactory::SP(new RoutableFactories60::RemoveLocationMessageFactory(*_repo)), from6);
    putRoutableFactory(MESSAGE_SEARCHRESULT, IRoutableFactory::SP(new RoutableFactories60::SearchResultMessageFactory()), from6);
    putRoutableFactory(MESSAGE_STATBUCKET, IRoutableFactory::SP(new RoutableFactories60::StatBucketMessageFactory()), from6);
    putRoutableFactory(MESSAGE_UPDATEDOCUMENT, IRoutableFactory::SP(new RoutableFactories60::UpdateDocumentMessageFactory(*_repo)), from6);
    putRoutableFactory(MESSAGE_VISITORINFO, IRoutableFactory::SP(new RoutableFactories60::VisitorInfoMessageFactory()), from6);
    putRoutableFactory(REPLY_CREATEVISITOR, IRoutableFactory::SP(new RoutableFactories60::CreateVisitorReplyFactory()), from6);
    putRoutableFactory(REPLY_DESTROYVISITOR, IRoutableFactory::SP(new RoutableFactories60::DestroyVisitorReplyFactory()), from6);
    putRoutableFactory(REPLY_DOCUMENTIGNORED, IRoutableFactory::SP(new RoutableFactories60::DocumentIgnoredReplyFactory()), from6);
    putRoutableFactory(REPLY_DOCUMENTLIST, IRoutableFactory::SP(new RoutableFactories60::DocumentListReplyFactory()), from6);
    putRoutableFactory(REPLY_DOCUMENTSUMMARY, IRoutableFactory::SP(new RoutableFactories60::DocumentSummaryReplyFactory()), from6);
    putRoutableFactory(REPLY_EMPTYBUCKETS, IRoutableFactory::SP(new RoutableFactories60::EmptyBucketsReplyFactory()), from6);
    putRoutableFactory(REPLY_GETBUCKETLIST, IRoutableFactory::SP(new RoutableFactories60::GetBucketListReplyFactory()), from6);
    putRoutableFactory(REPLY_GETBUCKETSTATE, IRoutableFactory::SP(new RoutableFactories60::GetBucketStateReplyFactory()), from6);
    putRoutableFactory(REPLY_GETDOCUMENT, IRoutableFactory::SP(new RoutableFactories60::GetDocumentReplyFactory(*_repo)), from6);
    putRoutableFactory(REPLY_MAPVISITOR, IRoutableFactory::SP(new RoutableFactories60::MapVisitorReplyFactory()), from6);
    putRoutableFactory(REPLY_PUTDOCUMENT, IRoutableFactory::SP(new RoutableFactories60::PutDocumentReplyFactory()), from6);
    putRoutableFactory(REPLY_QUERYRESULT, IRoutableFactory::SP(new RoutableFactories60::QueryResultReplyFactory()), from6);
    putRoutableFactory(REPLY_REMOVEDOCUMENT, IRoutableFactory::SP(new RoutableFactories60::RemoveDocumentReplyFactory()), from6);
    putRoutableFactory(REPLY_REMOVELOCATION, IRoutableFactory::SP(new RoutableFactories60::RemoveLocationReplyFactory()), from6);
    putRoutableFactory(REPLY_SEARCHRESULT, IRoutableFactory::SP(new RoutableFactories60::SearchResultReplyFactory()), from6);
    putRoutableFactory(REPLY_STATBUCKET, IRoutableFactory::SP(new RoutableFactories60::StatBucketReplyFactory()), from6);
    putRoutableFactory(REPLY_UPDATEDOCUMENT, IRoutableFactory::SP(new RoutableFactories60::UpdateDocumentReplyFactory()), from6);
    putRoutableFactory(REPLY_VISITORINFO, IRoutableFactory::SP(new RoutableFactories60::VisitorInfoReplyFactory()), from6);
    putRoutableFactory(REPLY_WRONGDISTRIBUTION, IRoutableFactory::SP(new RoutableFactories60::WrongDistributionReplyFactory()), from6);
}

DocumentProtocol::~DocumentProtocol() = default;

mbus::IRoutingPolicy::UP
DocumentProtocol::createPolicy(const mbus::string &name, const mbus::string &param) const
{
    return _routingPolicyRepository->createPolicy(name, param);
}

DocumentProtocol &
DocumentProtocol::putRoutingPolicyFactory(const string &name, IRoutingPolicyFactory::SP factory)
{
    _routingPolicyRepository->putFactory(name, factory);
    return *this;
}

mbus::Blob
DocumentProtocol::encode(const vespalib::Version &version, const mbus::Routable &routable) const
{
    mbus::Blob blob(_routableRepository->encode(version, routable));
        // When valgrind reports errors of uninitialized data being written to
        // the network, it is useful to be able to see the serialized data to
        // try to identify what bits are uninitialized.
    if (LOG_WOULD_LOG(spam)) {
        std::ostringstream message;
        document::StringUtil::printAsHex(
                message, blob.data(), blob.size());
        LOG(spam, "Encoded message of protocol %s type %u using version %s serialization:\n%s",
            routable.getProtocol().c_str(), routable.getType(),
            version.toString().c_str(), message.str().c_str());
    }
    return blob;
}

mbus::Routable::UP
DocumentProtocol::decode(const vespalib::Version &version, mbus::BlobRef data) const
{
    try {
        return _routableRepository->decode(version, data);
    } catch (vespalib::Exception &e) {
        LOG(warning, "%s", e.getMessage().c_str());
        return mbus::Routable::UP();
    }
}

uint32_t
DocumentProtocol::getRoutableTypes(const vespalib::Version &version, std::vector<uint32_t> &out) const
{
    return _routableRepository->getRoutableTypes(version, out);
}

DocumentProtocol &
DocumentProtocol::putRoutableFactory(uint32_t type, IRoutableFactory::SP factory,
                                     const vespalib::VersionSpecification &version)
{
    _routableRepository->putFactory(version, type, factory);
    return *this;
}

DocumentProtocol &
DocumentProtocol::putRoutableFactory(uint32_t type, IRoutableFactory::SP factory,
                                     const std::vector<vespalib::VersionSpecification> &versions)
{
    for (const auto & version : versions) {
        putRoutableFactory(type, factory, version);
    }
    return *this;
}

string
DocumentProtocol::getErrorName(uint32_t errorCode) {
    switch (errorCode) {
        case ERROR_MESSAGE_IGNORED:               return "MESSAGE_IGNORED";
        case ERROR_POLICY_FAILURE:                return "POLICY_FAILURE";
        case ERROR_DOCUMENT_NOT_FOUND:            return "DOCUMENT_NOT_FOUND";
        case ERROR_EXISTS:                        return "EXISTS";
        case ERROR_BUCKET_NOT_FOUND:              return "BUCKET_NOT_FOUND";
        case ERROR_BUCKET_DELETED:                return "BUCKET_DELETED";
        case ERROR_NOT_IMPLEMENTED:               return "NOT_IMPLEMENTED";
        case ERROR_ILLEGAL_PARAMETERS:            return "ILLEGAL_PARAMETERS";
        case ERROR_IGNORED:                       return "IGNORED";
        case ERROR_UNKNOWN_COMMAND:               return "UNKNOWN_COMMAND";
        case ERROR_UNPARSEABLE:                   return "UNPARSEABLE";
        case ERROR_NO_SPACE:                      return "NO_SPACE";
        case ERROR_INTERNAL_FAILURE:              return "INTERNAL_FAILURE";
        case ERROR_PROCESSING_FAILURE:            return "PROCESSING_FAILURE";
        case ERROR_TIMESTAMP_EXIST:               return "TIMESTAMP_EXIST";
        case ERROR_STALE_TIMESTAMP:               return "STALE_TIMESTAMP";
        case ERROR_NODE_NOT_READY:                return "NODE_NOT_READY";
        case ERROR_WRONG_DISTRIBUTION:            return "WRONG_DISTRIBUTION";
        case ERROR_REJECTED:                      return "REJECTED";
        case ERROR_ABORTED:                       return "ABORTED";
        case ERROR_BUSY:                          return "BUSY";
        case ERROR_NOT_CONNECTED:                 return "NOT_CONNECTED";
        case ERROR_DISK_FAILURE:                  return "DISK_FAILURE";
        case ERROR_IO_FAILURE:                    return "IO_FAILURE";
        case ERROR_SUSPENDED:                     return "SUSPENDED";
        case ERROR_TEST_AND_SET_CONDITION_FAILED: return "TEST_AND_SET_CONDITION_FAILED";
    }
    return mbus::ErrorCode::getName(errorCode);
}

void
DocumentProtocol::merge(mbus::RoutingContext &ctx)
{
    std::set<uint32_t> mask;
    merge(ctx, mask);
}

void
DocumentProtocol::merge(mbus::RoutingContext& ctx,
                        const std::set<uint32_t>& mask)
{
    ReplyMerger rm;
    uint32_t idx = 0;
    for (mbus::RoutingNodeIterator it = ctx.getChildIterator();
         it.isValid(); it.next(), ++idx)
    {
        if (mask.find(idx) != mask.end()) {
            continue;
        }
        rm.merge(idx, it.getReplyRef());
    }
    assert(idx != 0);
    ReplyMerger::Result res(rm.mergedReply());
    if (res.isSuccessful()) {
        const uint32_t okIdx = res.getSuccessfulReplyIndex();
        ctx.setReply(ctx.getChildIterator().skip(okIdx).removeReply()); 
    } else {
        assert(res.hasGeneratedReply());
        ctx.setReply(mbus::Reply::UP(res.releaseGeneratedReply().release()));
    }
}

bool
DocumentProtocol::hasOnlyErrorsOfType(const mbus::Reply &reply, uint32_t errCode)
{
    for (uint32_t i = 0; i < reply.getNumErrors(); ++i) {
        if (reply.getError(i).getCode() != errCode) {
            return false;
        }
    }
    return true;
}

}
