# signature-page
Programs to import phylogenetic tree and draw it on a tree or signature page

### Making tree for the signature page from the WHO CC sequence and HI tables

  - download sequences from gisaid (see GISAID.md) in put it into ~/ac/tables-store/sequences/gisaid-all-20160101-$(date +%Y%m%d).fas.bz2

  - make ~/AD/data/seqdb.json.xz (make sure hidb in ~/AD/data/ are up to date)

        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/seqdb-create --db ~/AD/data/seqdb.json.xz --match-hidb --clades ~/ac/tables-store/sequences/*.fas.bz2

  - mkdir $(date +%Y-%m%d-%H%M), copy seqdb

        D=$(date +%Y-%m%d-%H%M) && for V in bvic byam h1 h3; do mkdir /syn/eu/ac/results/whocc-tree/$V/$D; cp ~/AD/data/seqdb.json.xz /syn/eu/ac/results/whocc-tree/$V/$D; done

  - export sequences from ./seqdb.json.xz

    - base-seq. Use ~/AD/bin/seqdb-list --db ./seqdb.json.xz --re <name> to check, if the name below returns just one seq_id

        BVIC: "VICTORIA/830/2013 MDCK2"
        BYAM: "B/CHRISTCHURCH/503/2013 MDCK1" (since 2017-0214) "B/PHUKET/3073/2013 E4/E3" # "CAMBODIA/FSS29374/2014 MDCK1"
        H3:   "HAWAII/22/2012 MDCK"
        H1:   "SWITZERLAND/9772556/2013 SIAT2"

    - start-date

        H3: 20150301 (6686 sequences)

    - recent 4000

    - export command

        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/seqdb-export --db ./seqdb.json.xz --flu $VIRUS_TYPE --recent 4000 --hamming-distance-threshold 160 --tree-maker --base-seq $BASE_SEQ $WORKING_DIR/source.fas

        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/seqdb-export --db ./seqdb.json.xz --flu h3 --recent 4000 --hamming-distance-threshold 160 --tree-maker --base-seq "HAWAII/22/2012 MDCK" source.fas
        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/seqdb-export --db ./seqdb.json.xz --flu h1 --recent 4000 --hamming-distance-threshold 160 --tree-maker --base-seq "SWITZERLAND/9772556/2013 SIAT2" source.fas
        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/seqdb-export --db ./seqdb.json.xz --flu bvic --recent 4000 --hamming-distance-threshold 160 --tree-maker --base-seq "VICTORIA/830/2013 MDCK2" source.fas
        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/seqdb-export --db ./seqdb.json.xz --flu byam --recent 4000 --hamming-distance-threshold 160 --tree-maker --base-seq "B/CHRISTCHURCH/503/2013 MDCK1" source.fas

  - Initilialize tree maker

        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/tree-maker init

  - Edit $WORKING_DIR/tree-maker.config

  - Submit tree maker and wait for completion

        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/tree-maker wait

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

### Exporting recent merges from the acmacs server
