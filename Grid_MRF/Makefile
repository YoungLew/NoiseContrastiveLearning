CCFLAGS=CCFLAGS= -I/CODES/PhD/libDAI/include -I/opt/local/include -Wno-deprecated -Wall -w -Wextra -fPIC -DMACOSX -arch x86_64 -std=c++0x --stdlib=libc++ -lc++ -lc++abi  -O3 -g -DDAI_DEBUG  -DDAI_WITH_BP -DDAI_WITH_FBP -DDAI_WITH_TRWBP -DDAI_WITH_MF -DDAI_WITH_HAK -DDAI_WITH_LC -DDAI_WITH_TREEEP -DDAI_WITH_JTREE -DDAI_WITH_MR -DDAI_WITH_GIBBS -DDAI_WITH_CBP -DDAI_WITH_DECMAP -L/CODES/PhD/libDAI/lib -L/usr/local/Cellar/boost/1.57.0/lib -ldai -lgmpxx -lgmp -arch x86_64 -lpthread
all:
	g++ -o learn ./nce_learning.cxx $(CCFLAGS)
generate:
	g++ -o ./generate_data ./generate_data.cxx $(CCFLAGS)
