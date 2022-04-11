
#include "cjCorePCH.h"
#include "core/cjTList.h"


CJUT_DECL(CJCORE_API, cjCore, CJTHASH_TEST)



class TestHashPOD : public cjMem
{
	int		a;
	int		b;
	char	c;
};


static void TestHash()
{

	typedef cjTHash<cjAniSeqID, cjAniSeqRPtr> SeqIDToSeqRPtrHash;
	SeqIDToSeqRPtrHash						m_seqMap;

	cjAniSeqID tk = 0;
	cjAniSeqRPtr v;

	m_seqMap.Insert(tk, v);


	int k = 1;

	//
	// RefPtr 
	//

	typedef cjTHash<cjINT32 , cjNodeRPtr>		NodePtrHash;
	NodePtrHash			ptrdat;


	{
		cjNodeRPtr datRPtr = new cjNode;
		ptrdat.Insert(k, datRPtr);
	}

	ptrdat.RemoveAll();


	//
	// destructor call test
	//

	typedef cjTHash<cjINT32, cjAnimatorTimeline>		TimelineDatHash;
	TimelineDatHash	dat;

	cjAnimatorTimeline timeline;
	dat.Insert(k, timeline);
	dat.RemoveAll();


	
	// destructor POD
	

	typedef cjTHash<cjINT32, TestHashPOD>		TestHashPODHash;
	TestHashPODHash podDat;

	TestHashPOD pod;
	podDat.Insert(k, pod);
	podDat.RemoveAll();
	




	typedef cjTHash<int , const char*> TestHash;

	const char* t[] = { "aaa", "bbb", "ccc", "ddd", "eee", "fff", "ggg", "hhh", "iii", "jjj", "kkk", "lll", "mmm", "nnn", "ooo" };
	const int num = sizeof(t) /sizeof(t[0]);

	TestHash hash;

	for(int i=0; i<num; i++)
		 hash.Insert(i, t[i]);

	TestHash::Iter iter = hash.GetLast();
	TestHash::Pair* cur;

	cjLOG("\n* Begin 01 ********\n");

	//if(cur->m_key == 3)
	{
		int m=3;
		hash.Remove(m);

		int k=3;
		hash.Insert(k, "xxxx");
	}

	while(hash.GetNext(iter, cur))
	{		

		cjLOG("%3d - %s \n", cur->m_key, cur->m_val);
	}
		
	cjLOG("\n* End 01 ********\n\n");

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
//	TestList::Iterator iterByValFind = testLst.FindIter(t[5]);
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
//	TestList::Iterator insertIter = testLst.PushAfter(iterByValFind, "===inserted after #1===");
//	
//	//	add-null test
//	testLst.PushBack(0);
//
//	// remove insert
//	testLst.RemoveByIter(insertIter);
//
//	testLst.Remove(t[2]);
//		
//	return 0;

	// destructor call test

	/*{
		typedef cjTHash<void* , cjINT32 >  TestHashKV;

		TestHashKV   testKV;

		void* p = (void*)0x1234;
		cjINT32 x;

		testKV.Insert(p, x);

		TestHashKV::Iter iterKV = testKV.GetLast();
		void* rstP = cjNULL;

		while(testKV.GetNextKV(iterKV, rstP, cjNULL))
		{
			cjINT32 aa=0;
			aa=1;
		}

	}*/

}

CJUT_IMPL_BEGIN(CJCORE_API, cjCore, CJTHASH_TEST)

	TestHash();

CJUT_IMPL_END(CJCORE_API, cjCore, CJTHASH_TEST)

