# signature-page
Programs to import phylogenetic tree and draw it on a tree or signature page

### Making tree for the signature page from the WHO CC sequence and HI tables

  - download sequences from gisaid (see section Gisaid below) in put it into ~/ac/tables-store/sequences/$FASTA_NAME

        FASTA_NAME=gisaid-all-$(date +%Y%m%d).fas.bz2

  - make ~/AD/data/seqdb.json.xz (make sure hidb in ~/AD/data/ are up to date)

        env LD_LIBRARY_PATH=$HOME/AD/lib ~/AD/bin/seqdb-create --db ~/AD/data/seqdb.json.xz --match-hidb --clades ~/ac/tables-store/sequences/gisaid-b-1997-20160211.fas.bz2 ~/ac/tables-store/sequences/gisaid-h3-1998-20160211.fas.bz2 ~/ac/tables-store/sequences/gisaid-h1pdm-2009-20160211.fas.bz2 ~/ac/tables-store/sequences/$FASTA_NAME ~/ac/tables-store/sequences/niid-*.fas.bz2

  - mkdir $WORKING_DIR

        WORKING_DIR="/syn/eu/ac/results/whocc-tree/$VIRUS_TYPE/$(date +%Y-%m%d-%H%M)"
        mkdir "$WORKING_DIR"
        cd "$WORKING_DIR"

  - export sequences from ~/AD/data/seqdb.json.xz

    - base-seq:

        BVIC: "VICTORIA/830/2013 MDCK2",
        BYAM: "B/PHUKET/3073/2013 E4/E2", # "CAMBODIA/FSS29374/2014 MDCK1", # David used for 201602 SSM B/HUMAN/PHUKET/3073/2013%20E4/E1/SPF12%20%2A%20CDC-LV11B%20%28%20YAMAGATA-2013
        H3:   "HAWAII/22/2012 MDCK",
        H1:   "SWITZERLAND/9772556/2013 SIAT2",

    - start-date

        H3: 20150301 (6686 sequences)

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
