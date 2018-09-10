#include "acmacs-base/json-writer.hh"
#include "acmacs-base/json-reader.hh"
namespace jsw = json_writer;

#include "acmacs-base/enumerate.hh"
#include "acmacs-base/read-file.hh"
#include "seqdb/seqdb.hh"
#include "signature-page/tree-export.hh"
#include "signature-page/tree.hh"

// ----------------------------------------------------------------------

static constexpr const char* TREE_NEWICK_VERSION = "newick-tree-v1";
static constexpr const char* TREE_PHYLOGENETIC_VERSION = "phylogenetic-tree-v2";

// ----------------------------------------------------------------------

enum class TreeJsonKey : char
{
    Subtree='t', SeqId='n', EdgeLength='l',

    Unknown='?'
};

template <typename RW> inline json_writer::writer<RW>& operator<<(json_writer::writer<RW>& aWriter, TreeJsonKey aKey)
{
    const char k = static_cast<char>(aKey);
    aWriter.Key(&k, 1, false);
    return aWriter;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const Node& aNode)
{
    return writer << jsw::start_object
                  << jsw::if_not_empty(TreeJsonKey::SeqId, aNode.seq_id)
                  << jsw::if_non_negative(TreeJsonKey::EdgeLength, aNode.edge_length)
                  << jsw::if_not_empty(TreeJsonKey::Subtree, aNode.subtree)
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const Tree& aTree)
{
    return writer << jsw::start_object
                  << jsw::key("  version") << TREE_PHYLOGENETIC_VERSION
                  << jsw::key("tree") << static_cast<const Node&>(aTree)
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

void tree::export_to_json(std::string aFilename, const Tree& aTree, size_t aIndent)
{
    jsw::export_to_json(aTree, aFilename, aIndent);

} // tree::export_to_json

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
                      result = new json_reader::ListHandler<Node, Node, PhylogeneticV2Handler>(mTarget, mTarget.subtree);
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

    inline PhylogeneticV2Handler(Node& , Node& aTarget) // for json_reader::ListHandler
        : HandlerBase(aTarget), mKey(TreeJsonKey::Unknown) {}

    friend class json_reader::ListHandler<Node, Node, PhylogeneticV2Handler>;
};

// ----------------------------------------------------------------------

class TreeRootHandler : public HandlerBase
{
 private:
    enum class Keys { Unknown, Version, Tree };
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
                mKey = Keys::Tree;
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
                      std::cerr << "ERROR: Unsupported version: \"" << std::string(str, length) << '"' << std::endl;
                      throw json_reader::Failure();
                  }
                  break;
              case Keys::Tree:
              case Keys::Unknown:
                  result = HandlerBase::String(str, length);
            }
            return result;
        }

        inline virtual HandlerBase* StartObject()
            {
                HandlerBase* result = nullptr;
                switch (mKey) {
                  case Keys::Tree:
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
                      break;
                  case Keys::Unknown:
                  case Keys::Version:
                      result = HandlerBase::StartObject();
                      break;
                }
                return result;
            }

 private:
    Keys mKey;
    TreeType mTreeType;
};

// ----------------------------------------------------------------------

void tree::tree_import(std::string aFilename, Tree& aTree)
{
    json_reader::read_from_file<Node, TreeRootHandler>(aFilename, aTree);
      // aTree.set_number_strains();

} // tree::tree_import

// ----------------------------------------------------------------------

Tree tree::tree_import(std::string aFilename)
{
    Tree tree;
    tree_import(aFilename, tree);
    return tree;
}

// ----------------------------------------------------------------------

Tree tree::tree_import(std::string aFilename, std::shared_ptr<acmacs::chart::Chart> chart, Tree::LadderizeMethod aLadderizeMethod)
{
    Tree tree;
    tree_import(aFilename, tree);
    tree.match_seqdb(seqdb::get());
    if (chart) {
        const auto matched_names = tree.match(*chart);
        if (matched_names)
            std::cout << "INFO: tree sequences found in the chart: " << matched_names << std::endl;
    }
    tree.set_number_strains();
    tree.ladderize(aLadderizeMethod);
    tree.compute_cumulative_edge_length();
    return tree;

} // tree::tree_import

// ----------------------------------------------------------------------

// https://en.wikipedia.org/wiki/Newick_format
void tree::export_to_newick(std::string aFilename, const Tree& aTree, size_t aIndent) {
    std::string result;
    std::function<void(const Node &, bool, size_t)> export_node;
    export_node = [&result, &export_node, aIndent](const Node& node, bool comma, size_t indent) {
        result.append(indent, ' ');
        if (node.subtree.empty()) {
            result.append(node.seq_id);
        }
        else {
            result.append(1, '(');
            result.append(1, '\n');
            for (auto [no, subnode] : acmacs::enumerate(node.subtree))
                export_node(subnode, no != (node.subtree.size() - 1), indent + aIndent);
            result.append(indent, ' ');
            result.append(1, ')');
        }
        if (!float_zero(node.edge_length)) {
            result.append(1, ':');
            result.append(acmacs::to_string(node.edge_length));
        }
        if (comma)
            result.append(1, ',');
        result.append(1, '\n');
    };

    export_node(aTree, false, 0);
    result.append(1, ';');
    acmacs::file::write(aFilename, result);

} // tree::export_to_newick

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
