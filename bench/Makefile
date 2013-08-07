
BENCHMARKS = 501.motioncontrol 502.linearcode 503.slots 504.intjitter
.PHONY: all $(BENCHMARKS) common

all: $(BENCHMARKS) common
	@make -C common; for bench in $(BENCHMARKS) ; do make -C $$bench test ; done

run:
	@for bench in $(BENCHMARKS) ; do make -C $$bench run ; done

clean:
	@find . -name "*.x" -exec rm {} \;
	@find . -name "*.o" -exec rm {} \;

