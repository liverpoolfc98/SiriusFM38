CFLAGS = -lm -c -g -O3 -march=native -mtune=native -DNDEBUG
MAIN = Test5

a.out: $(MAIN).o IRProvider.o IRProviderConst.o
	g++ $^ -o a.out
$(MAIN).o: $(MAIN).cpp MCOptionHedger1D.hpp MCOptionHedger1D.h MCOptionPricer1D.hpp MCOptionPricer1D.h IRProviderConst.h VanillaOptions.h MCEngine1D.hpp MCEngine1D.h IRProvider.h DiffusionGBM.h DiffusionCEV.h DiffusionCIR.h DiffusionLipton.h DiffusionOU.h
	g++ $(CFLAGS) $<
IRProvider.o: IRProvider.cpp 
	g++ $(CFLAGS) $<
IRProviderConst.o: IRProviderConst.cpp IRProvider.h
	g++ $(CFLAGS) $<
clean:
	rm -rf *.o
