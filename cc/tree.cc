#include "tree.hh"
#include "tree-iterate.hh"

// ----------------------------------------------------------------------

void Tree::match_seqdb(const seqdb::Seqdb& seqdb)
{
    auto match = [&seqdb](Node& node) {
        node.data.assign(seqdb.find_by_seq_id(node.seq_id));
    };
    tree::iterate_leaf(*this, match);

} // Tree::match_seqdb

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
