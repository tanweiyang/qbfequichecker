
## Compile options
CFLAGS = -Wall -O0 -g -pg --static -Wno-sign-compare -Wno-unused-variable
CFLAGS = -Wall -O0 -g --static -Wno-sign-compare -Wno-unused-variable
CFLAGS = -Wall -O5 --static -Wno-sign-compare -Wno-unused-variable
CFLAGS = -Wall -I./ -g -Wno-sign-compare -Wno-unused-variable

#cirqit: Allocator.cpp  Circuit.cpp  DagNode.cpp    LeafNode.cpp    Literal.cpp      NodeFactory.h  Parser.cpp      QdToQpro.cpp  Solver.cpp  UndoRecord.cpp Allocator.h    Circuit.h    DagNode.h      LeafNode.h      Literal.h        NotGate.cpp    Parser.h        QdToQpro.h    Solver.h    UndoRecord.h AndOrGate.cpp  Common.cpp   DummyTree.cpp  LearningDB.cpp  main.cpp         NotGate.h      PropRecord.cpp  Reason.cpp    Tester.cpp AndOrGate.h    Common.h     DummyTree.h    LearningDB.h    NodeFactory.cpp  Notifiable.h   PropRecord.h    Reason.h      Tester.h
#   g++ $(CFLAGS) -o cirqit *.cpp


cirqit : libcirqit.a main.o 
	g++ $(CFLAGS) -o $@ $^


main.o : main.cpp
	g++ $(CFLAGS) -c $^ -o $@


libcirqit.a: Allocator.cpp  Circuit.cpp  DagNode.cpp    LeafNode.cpp    Literal.cpp      NodeFactory.h  Parser.cpp      QdToQpro.cpp  Solver.cpp  UndoRecord.cpp Allocator.h    Circuit.h    DagNode.h      LeafNode.h      Literal.h        NotGate.cpp    Parser.h        QdToQpro.h    Solver.h    UndoRecord.h AndOrGate.cpp  Common.cpp   DummyTree.cpp  LearningDB.cpp   NotGate.h      PropRecord.cpp  Reason.cpp    Tester.cpp AndOrGate.h    Common.h     DummyTree.h    LearningDB.h    NodeFactory.cpp  Notifiable.h   PropRecord.h    Reason.h      Tester.h
	g++ $(CFLAGS) $(addprefix -c ,$(filter %.cpp,$^))
	ar rv $@ $(^:.cpp=.o)  

clean:
	rm -f cirqit 
	rm -f *.o
	rm -f libcirqit.a
 
