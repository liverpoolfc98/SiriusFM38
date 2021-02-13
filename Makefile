CFLAGS = -lm -c -g -O3 -march=native -DNDEBUG
MAIN = Test3
COMPILER = g++

a.out: $(MAIN).o IRProvider.o IRProviderConst.o
	$(COMPILER) -fopenmp $^ -o a.out
$(MAIN).o: $(MAIN).cpp MCOptionHedger1D.hpp MCOptionHedger1D.h MCOptionPricer1D.hpp MCOptionPricer1D.h IRProviderConst.h VanillaOptions.h MCEngine1D.hpp MCEngine1D.h IRProvider.h DiffusionGBM.h DiffusionCEV.h DiffusionCIR.h DiffusionLipton.h DiffusionOU.h
	$(COMPILER) $(CFLAGS) -fopenmp $<
IRProvider.o: IRProvider.cpp 
	$(COMPILER) $(CFLAGS) -fopenmp $<
IRProviderConst.o: IRProviderConst.cpp IRProvider.h
	$(COMPILER) $(CFLAGS) -fopenmp $<
clean:
	rm -rf *.o
