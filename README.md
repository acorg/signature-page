# signature-page
Programs to import phylogenetic tree and draw it on a tree or signature page

### Making tree for the signature page from the WHO CC sequence and HI tables

  - download sequences from gisaid (see section Gisaid below) in put it into ~/ac/tables-store/sequences/$FASTA_NAME

        FASTA_NAME=gisaid-all-$(date +%Y%m%d).fas.bz2

  - make ~/AD/data/seqdb.json.xz (make sure hidb in ~/AD/data/ are up to date)

        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/seqdb-create --db ~/AD/data/seqdb.json.xz --match-hidb --clades ~/ac/tables-store/sequences/$FASTA_NAME ~/ac/tables-store/sequences/niid-*.fas.bz2

  - mkdir $WORKING_DIR

        WORKING_DIR="/syn/eu/ac/results/whocc-tree/$VIRUS_TYPE/$(date +%Y-%m%d-%H%M);"
        mkdir "$WORKING_DIR"
        cd "$WORKING_DIR"

  - export sequences from ~/AD/data/seqdb.json.xz

        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/seqdb-export --db ~/AD/data/seqdb.json.xz --flu $VIRUS_TYPE --start-date YYYYMMDD --tree-maker --base-seq $BASE_SEQ $WORKING_DIR/source.fas

  - Initilialize tree maker

        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/tree-maker init

  - Edit $WORKING_DIR/tree-maker.config

  - Submit tree maker and wait for completion

        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/tree-maker wait

### Gisaid

  - visit [http://gisaid.org](http://gisaid.org) and login (at right)
  - Click EpiFlu (top middle)
  - Hold Shift and click A and B in the Type.
  - Set collection date From to 2014-02-01 (or later depending on the number of isolates upon search, see below)
  - _Leave originating lab empty!_

  - Select **Submitting lab** them all using Cmd

    (Change height of the submitting lab <select> 500px)

         [Australia, North Melbourne] WHO CC ...
         [United Kingdom, London] National Institute for Medical Research
         [United Kingdom, London] Crick
         [United states, Atlanta] CDC
         ? [China, Bejing] WHO Chinese National Influenza Center

  - Required segments: HA
  - Click Search. Total must not exceed 20k isolates (gisaid does not allow downloading more)
  - Click leftmost checkbox in header to select all
  - Click Download
  - Format: Sequences (DNA) as FASTA
  - Segment: HA
  - FASTA Header: Isolate name |  Collection date | Passage details/history |  Sample ID by submitting lab | Submitting lab | Type |  Lineage
  - Date format: YYYY-MM-DD
  - Click download

  - Rename file:

        dos2unix ~/Downloads/gisaid_epiflu_sequence.fasta
        mv ~/Downloads/gisaid_epiflu_sequence.fasta ~/Downloads/gisaid-all-$(date +%Y%m%d).fas
        bzip2 -9v ~/Downloads/gisaid-all-$(date +%Y%m%d).fas
        mv ~/Downloads/gisaid-all-$(date +%Y%m%d).fas.bz2 ~/ac/tables-store/sequences
        ssax
