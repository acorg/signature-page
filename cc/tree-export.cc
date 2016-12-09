#include "tree-export.hh"
#include "acmacs-base/json-reader.hh"
#include "tree.hh"

// ----------------------------------------------------------------------

static constexpr const char* TREE_NEWICK_VERSION = "newick-tree-v1";
static constexpr const char* TREE_PHYLOGENETIC_VERSION = "phylogenetic-tree-v2";

// ----------------------------------------------------------------------

enum class TreeJsonKey : char
{
    Subtree='t', SeqId='n', EdgeLength='l',

    Unknown
};

// ----------------------------------------------------------------------

using HandlerBase = json_reader::HandlerBase<Node>;

// ----------------------------------------------------------------------

class NewickNodeHandler : public HandlerBase
{
 public:
    inline NewickNodeHandler(Node& aTarget)
        : HandlerBase(aTarget), mKey(TreeJsonKey::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            if (length == 1) {
                mKey = static_cast<TreeJsonKey>(*str);
#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif
                switch (mKey) {
                  case TreeJsonKey::EdgeLength:
                  case TreeJsonKey::SeqId:
                      break;
                  case TreeJsonKey::Subtree:
                      result = new json_reader::ListHandler<Node, Node, NewickNodeHandler>(mTarget, mTarget.subtree);
                      break;
                  default:
                      result = HandlerBase::Key(str, length);
                      break;
                }
#pragma GCC diagnostic pop
            }
            else {
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* Double(double d)
        {
            if (mKey != TreeJsonKey::EdgeLength)
                throw json_reader::Failure();
            mTarget.edge_length = d;
            return nullptr;
        }

    inline virtual HandlerBase* String(const char* str, rapidjson::SizeType length)
        {
            if (mKey != TreeJsonKey::SeqId)
                throw json_reader::Failure();
            mTarget.seq_id.assign(str, length);
            return nullptr;
        }

 private:
    TreeJsonKey mKey;

    inline NewickNodeHandler(Node& , Node& aTarget) // for json_reader::ListHandler
        : HandlerBase(aTarget), mKey(TreeJsonKey::Unknown) {}

    friend class json_reader::ListHandler<Node, Node, NewickNodeHandler>;
};

// ----------------------------------------------------------------------

class PhylogeneticV2Handler : public HandlerBase
{
 public:
    inline PhylogeneticV2Handler(Node& aTarget)
        : HandlerBase(aTarget) {}
};

// ----------------------------------------------------------------------

class TreeRootHandler : public HandlerBase
{
 private:
    enum class Keys { Unknown, Version };
    enum class TreeType { Unknown, Newick, PhylogeneticV2 };

 public:
    inline TreeRootHandler(Node& aTree) : HandlerBase{aTree}, mKey(Keys::Unknown), mTreeType(TreeType::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            const std::string found_key(str, length);
            if (found_key == "  version") {
                mKey = Keys::Version;
            }
            else if (found_key == "tree") {
                switch (mTreeType) {
                  case TreeType::Newick:
                      result = new NewickNodeHandler(mTarget);
                      break;
                  case TreeType::PhylogeneticV2:
                      result = new PhylogeneticV2Handler(mTarget);
                      break;
                  case TreeType::Unknown:
                      throw json_reader::Failure();
                }
            }
            else {
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* String(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::Version:
                  if (!strncmp(str, TREE_NEWICK_VERSION, std::min(length, static_cast<rapidjson::SizeType>(strlen(TREE_NEWICK_VERSION))))) {
                      mTreeType = TreeType::Newick;
                  }
                  else if (!strncmp(str, TREE_PHYLOGENETIC_VERSION, std::min(length, static_cast<rapidjson::SizeType>(strlen(TREE_PHYLOGENETIC_VERSION))))) {
                      mTreeType = TreeType::PhylogeneticV2;
                  }
                  else {
                      std::cerr << "Unsupported version: \"" << std::string(str, length) << '"' << std::endl;
                      throw json_reader::Failure();
                  }
                  break;
              case Keys::Unknown:
                  result = HandlerBase::String(str, length);
            }
            return result;
        }

 private:
    Keys mKey;
    TreeType mTreeType;
};

// ----------------------------------------------------------------------

void read_tree(std::string aFilename, Tree& aTree)
{
    json_reader::read_from_file<Node, TreeRootHandler>(aFilename, aTree);
}

// ----------------------------------------------------------------------
