CFLAGS = -lm -c -g -O3
MAIN = Test2

a.out: $(MAIN).o IRProvider.o IRProviderConst.o
	g++ $^ -o a.out
$(MAIN).o: $(MAIN).cpp IRProviderConst.h VanillaOptions.h MCEngine1D.hpp MCEngine1D.h IRProvider.h DiffusionGBM.h DiffusionCEV.h DiffusionCIR.h DiffusionLipton.h DiffusionOU.h
	g++ $(CFLAGS) $<
IRProvider.o: IRProvider.cpp
	g++ $(CFLAGS) $<
IRProviderConst.o: IRProviderConst.cpp IRProvider.h
	g++ $(CFLAGS) $<
clean:
	rm -rf *.o
