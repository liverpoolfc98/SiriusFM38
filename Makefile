CFLAGS = -lm -c -g -O3 -march=native -DNDEBUG
MAIN = Test5
COMPILER = g++
PARFLAG = -ta=tesla:managed -acc
PAR = -fopenmp

a.out: $(MAIN).o IRProvider.o IRProviderConst.o
	$(COMPILER) $(PAR) $^ -o a.out
$(MAIN).o: $(MAIN).cpp GridNOP1D_S3_RKC1.hpp MCOptionHedger1D.hpp MCOptionHedger1D.h MCOptionPricer1D.hpp MCOptionPricer1D.h IRProviderConst.h VanillaOptions.h MCEngine1D.hpp MCEngine1D.h IRProvider.h DiffusionGBM.h DiffusionCEV.h DiffusionCIR.h DiffusionLipton.h DiffusionOU.h
	$(COMPILER) $(CFLAGS) $(PAR) $<
IRProvider.o: IRProvider.cpp 
	$(COMPILER) $(CFLAGS) $(PAR) $<
IRProviderConst.o: IRProviderConst.cpp IRProvider.h
	$(COMPILER) $(CFLAGS) $(PAR) $<
clean:
	rm -rf *.o
