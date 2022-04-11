
#include "cjCorePCH.h"


cjUINT32 CJUT_TEST_cjTNullableAry()
{
	typedef cjTNullableAry<const char*> TestNullableAry;

	TestNullableAry testAry;

	const char* t[] = { "aaa", "bbb", "ccc", "ddd", "eee", "1aaa", "1bbb", "1ccc", "1ddd", "1eee", "2aaa", "2bbb", "2ccc", "2ddd", "2eee", "3aaa", "3bbb", "3ccc", "3ddd", "3eee" };
	const int tSiz = sizeof(t) /sizeof(t[0]);

	for(int i=0; i<tSiz; i++)
		testAry.Push(t[i]);

	// remove test
	testAry.RemoveAll();

		// remove-refill
	for(int i=0; i<(tSiz); i++)
		testAry.Push(t[i]);

		// decrement num element when removing last element at full filled
	testAry.Remove(testAry.Num()-1);

		// add-null test
	testAry.Push(0);
	testAry.Push(0);

		// add item at first no-null
	testAry.Push("first-no-null");

		// remove index 2 = ccc
	testAry.Remove(2);
	const char* testAry_index_02 = testAry(2);

		// insert first no-null
	testAry.Push("---ccc---");

	cjAryOff x = testAry.Last();

		// shrink
	testAry.Shrink();

	TestNullableAry duppAry;

		// copy test
	duppAry = testAry;


	TestNullableAry duppAry2(testAry);
	
	// insert after shrink
	testAry.Push("shrink");

	// ref ptr test
	typedef cjTNullableAry<cjAniCntrSeqRPtr>	AniCntrSeqPtrAry;
	AniCntrSeqPtrAry csAry;

	

	
	return 0;
}

