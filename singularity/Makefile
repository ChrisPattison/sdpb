CONTAINERS=sdpb.sif base.sif
SINGULARITY=sudo singularity

.PHONY: all
all: sdpb.sif

.PHONY: clean
clean:
	rm -f ${CONTAINERS}

%.sif: %.def
	$(SINGULARITY) build --force --notest $@ $<

sdpb.sif: base.sif $(shell find ../src -type f)
