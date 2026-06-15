PYTHON := python3
CODE    := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

.PHONY: phase0 phase1 phase2 phase3 phase4 phase5 all clean

phase0:
	$(PYTHON) $(CODE)/phase0_sanity.py

phase1:
	$(PYTHON) $(CODE)/phase1_dataset.py

phase2:
	$(PYTHON) $(CODE)/phase2_features.py

phase3:
	$(PYTHON) $(CODE)/phase3_freertos.py

phase4:
	$(PYTHON) $(CODE)/phase4_analysis.py

phase5:
	$(PYTHON) $(CODE)/phase5_falsify.py

all: phase0 phase1 phase2 phase3 phase4 phase5

clean:
	rm -f ../data/gt_assertions*.csv ../data/phase*.json
	rm -f ../figures/phase*.png ../figures/*.pdf
