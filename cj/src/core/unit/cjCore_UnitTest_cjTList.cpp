
#include "cjCorePCH.h"
#include "core/cjTList.h"

//
//class TestListPOD : public cjMem
//{
//	int		a;
//	int		b;
//	char	c;
//};
//
//
//
//CJCORE_API cjUINT32 CJUT_TEST_cjTList()
//{
//	//
//	// RefPtr 
//	//
//
//	typedef cjTList<cjNodeRPtr>		NodePtrLst;
//	NodePtrLst			ptrdat;
//
//	{
//		cjNodeRPtr datRPtr = new cjNode;
//		rptrdat.PushBack(datRPtr);
//	}
//
//	rptrdat.RemoveAll();
//
//
//	//
//	// destructor call test
//	//
//
//	typedef cjTList<cjAnimatorTimeline>		TimelineDatLst;
//	TimelineDatLst	dat;
//
//	cjAnimatorTimeline timeline;
//	dat.PushBack(timeline);
//
//	dat.RemoveAll();
//
//
//	
//	// destructor POD
//	
//
//	typedef cjTList<TestListPOD>		TestListPODLst;
//	TestListPODLst podDat;
//
//	TestListPOD pod;
//	podDat.PushBack(pod);
//	podDat.RemoveAll();
//
//
//
//
//
//
//	typedef cjTList<const char*> TestList;
//
//	TestList testLst;
//
//	const char* t[] = { "aaa", "bbb", "ccc", "ddd", "eee", "1aaa", "1bbb", "1ccc", "1ddd", "1eee", "2aaa", "2bbb", "2ccc", "2ddd", "2eee", "3aaa", "3bbb", "3ccc", "3ddd", "3eee" };
//	const int tSiz = sizeof(t) /sizeof(t[0]);
//
//	for(int i=0; i<tSiz; i++)
//		testLst.PushBack(t[i]);
//
//	// remove test
//	testLst.RemoveAll();
//
//	// remove-refill
//	for(int i=0; i<(tSiz); i++)
//		testLst.PushBack(t[i]);
//
//
//	// find item
//	TestList::Iter iterByValFind = testLst.Find(t[5]);
//	cjAssert(iterByValFind);
//
//	const char* val = 0;
////	val = testLst.GetValue(iterByValFind);
//	val = *(*iterByValFind);
//
//	// insert after
//	testLst.PushBefore(iterByValFind, "===inserted before #0===");
//
//	// insert after
//	TestList::Iter insertIter = testLst.PushAfter(iterByValFind, "===inserted after #1===");
//	
//	//	add-null test
//	testLst.PushBack(0);
//
//	// remove insert
//	testLst.RemoveByIter(insertIter);
//
//	testLst.Remove(t[2]);
//	
//	
//
//
//
//
//	return 0;
//}
//
