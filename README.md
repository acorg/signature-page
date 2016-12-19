# signature-page
Programs to import phylogenetic tree and draw it on a tree or signature page

# Making tree for the signature page from the WHO CC sequence and HI tables

  - download sequences from gisaid in put it into ~/ac/tables-store/sequences/$FASTA_NAME

        FASTA_NAME=gisaid-all-$(date +%Y%m%d).fas.bz2

  - make ~/WHO/seqdb.json.xz

        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/seqdb-create --db ~/WHO/seqdb.json.xz --match-hidb --clades ~/ac/tables-store/sequences/gisaid-all-20160914.fas.bz2 ~/ac/tables-store/sequences/niid-*.fas.bz2

  - mkdir $WORKING_DIR

        WORKING_DIR="/syn/eu/ac/results/whocc-tree/$VIRUS_TYPE/$(date +%Y-%m%d-%H%M);"
        mkdir "$WORKING_DIR"
        cd "$WORKING_DIR"

  - export sequences from ~/WHO/seqdb.json.xz
