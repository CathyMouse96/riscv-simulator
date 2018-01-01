# build Simulator
all: Simulator.cpp MulticycleSimulator.cpp PipelineSimulator.cpp ReadElf.cpp
	g++ -g Simulator.cpp ReadElf.cpp -o Simulator
	g++ -g MulticycleSimulator.cpp ReadElf.cpp -o MulticycleSimulator
	g++ -g PipelineSimulator.cpp ReadElf.cpp -o PipelineSimulator
clean:
	$(RM) Simulator MulticycleSimulator PipelineSimulator ReadElf
single: Simulator.cpp ReadElf.cpp
	g++ -g Simulator.cpp ReadElf.cpp -o Simulator
multi: MulticycleSimulator.cpp ReadElf.cpp
	g++ -g MulticycleSimulator.cpp ReadElf.cpp -o MulticycleSimulator
pipe: PipelineSimulator.cpp ReadElf.cpp
	g++ -g PipelineSimulator.cpp ReadElf.cpp -o PipelineSimulator
