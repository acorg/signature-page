# signature-page
Programs to import phylogenetic tree and draw it on a tree or signature page

### Making tree for the signature page from the WHO CC sequence and HI tables

  see acmacs-whocc/doc/signature-page.org

### Drawing tree

  - Init settings

        ~/AD/bin/sigp --seqdb ~/AD/data/seqdb.json.xz --init-settings tree.settings.json tree.json.xz /tmp/tree.pdf && open /tmp/tree.pdf

  - Edit settings in tree.settings.json

  - Generate pdf with the tree

        ~/AD/bin/sigp --seqdb ~/AD/data/seqdb.json.xz -s tree.settings.json tree.json.xz tree.pdf && open tree.pdf

### Signature page

  - Init settings

        ~/AD/bin/sigp --seqdb ~/AD/data/seqdb.json.xz --chart <chart.sdb> --init-settings sigp.settings.json tree.json.xz /tmp/sigp.pdf && open /tmp/sigp.pdf

  - Edit settings in sigp.settings.json

  - Generate pdf with the signature page

        ~/AD/bin/sigp --seqdb ~/AD/data/seqdb.json.xz --chart <chart.sdb> -s sigp.settings.json tree.json.xz sigp.pdf && open sigp.pdf
