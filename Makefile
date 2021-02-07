CFLAGS = -lm -c -g
a.out: main.o IRProvider.o IRProviderConst.o
	g++ $^ -o a.out
main.o: main.cpp IRProviderConst.h IRProvider.h DiffusionGBM.h DiffusionCEV.h DiffusionCIR.h DiffusionLipton.h DiffusionOU.h
	g++ $(CFLAGS) $<
IRProvider.o: IRProvider.cpp
	g++ $(CFLAGS) $<
IRProviderConst.o: IRProviderConst.cpp IRProvider.h
	g++ $(CFLAGS) $<
clean:
	rm -rf *.o
