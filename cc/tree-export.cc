#include "tree-export.hh"
#include "acmacs-base/json-reader.hh"
#include "tree.hh"

// ----------------------------------------------------------------------

static constexpr const char* TREE_NEWICK_VERSION = "newick-tree-v1";
static constexpr const char* TREE_PHYLOGENETIC_VERSION = "phylogenetic-tree-v2";

// ----------------------------------------------------------------------

using HandlerBase = json_reader::HandlerBase<Tree>;

// ----------------------------------------------------------------------

class NewickHandler : public HandlerBase
{
 public:
    inline NewickHandler(Tree& aTarget)
        : HandlerBase(aTarget) {}

};

// ----------------------------------------------------------------------

class PhylogeneticV2Handler : public HandlerBase
{
 public:
    inline PhylogeneticV2Handler(Tree& aTarget)
        : HandlerBase(aTarget) {}
};

// ----------------------------------------------------------------------

class TreeRootHandler : public HandlerBase
{
 private:
    enum class Keys { Unknown, Version };
    enum class TreeType { Unknown, Newick, PhylogeneticV2 };

 public:
    inline TreeRootHandler(Tree& aTree) : HandlerBase{aTree}, mKey(Keys::Unknown), mTreeType(TreeType::Unknown) {}

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
                      result = new NewickHandler(mTarget);
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
                  break;
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
    json_reader::read_from_file<Tree, TreeRootHandler>(aFilename, aTree);
}

// ----------------------------------------------------------------------
