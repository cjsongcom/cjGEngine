
#ifndef _CJCOREAUTOREGCLASSES_H_
#define _CJCOREAUTOREGCLASSES_H_


#ifdef CJDLL
	#define CJCORE_AUTOREG_CLASSES 
#else

	#define CJCORE_AUTOREG_CLASSES \
		cjObjPackage::cjGetStaticCls(); \
		cjElement::cjGetStaticCls(); \
		cjProperty::cjGetStaticCls(); \
		cjBoolProperty::cjGetStaticCls(); \
		cjIntProperty::cjGetStaticCls(); \
		cjObjProperty::cjGetStaticCls(); \
		cjObjClsProperty::cjGetStaticCls(); \
		cjAryProperty::cjGetStaticCls(); \
		cjSerObjRefProperty::cjGetStaticCls(); \
		cjPropStruct::cjGetStaticCls(); \
		cjObjCls::cjGetStaticCls(); \
		cjObjFactory::cjGetStaticCls(); \
		cjObjPkgLinker::cjGetStaticCls(); \
		cjObjPkgLoad::cjGetStaticCls(); \
		cjObjPkgSave::cjGetStaticCls(); \
		cjResAniSeq::cjGetStaticCls(); \
		\
		cjAniColorDat::cjGetStaticCls(); \
		cjAniPoseBind::cjGetStaticCls(); \
		cjAniPoseBuf::cjGetStaticCls(); \
		cjAniMTargetPoseHandler::cjGetStaticCls(); \
		cjAniPoseBlend::cjGetStaticCls(); \
		cjAniSeq::cjGetStaticCls(); \
		cjAniSPad::cjGetStaticCls(); \
		cjVisibleAniCntr::cjGetStaticCls(); \
		cjTransformAniCntr::cjGetStaticCls(); \
		cjExtDatBool::cjGetStaticCls(); \
		cjExtDatInt::cjGetStaticCls(); \
		cjExtDatStr::cjGetStaticCls(); \
		cjExtDatAniNote::cjGetStaticCls(); \
		cjExtDatBinary::cjGetStaticCls(); \
		cjExtDatVector::cjGetStaticCls(); \
		cjExtDatMatrix::cjGetStaticCls(); \
		cjInterpolator::cjGetStaticCls(); \
		cjLookAtInterpolator::cjGetStaticCls(); \
		cjFloatInterpolator::cjGetStaticCls(); \
		cjBoolInterpolator::cjGetStaticCls(); \
		cjBoolTimelineInterpolator::cjGetStaticCls(); \
		cjVec3SInterpolator::cjGetStaticCls(); \
		cjVec3PInterpolator::cjGetStaticCls(); \
		cjQuatInterpolator::cjGetStaticCls(); \
		cjTMInterpolator::cjGetStaticCls(); \
		cjPathInterpolator::cjGetStaticCls(); \
		cjColorInterpolator::cjGetStaticCls(); \
		cjBlendTMInterpolator::cjGetStaticCls(); \
		cjBlendBoolInterpolator::cjGetStaticCls(); \
		cjBSColorInterpolator::cjGetStaticCls(); \
		cjBSFloatInterpolator::cjGetStaticCls(); \
		cjBSVec3SInterpolator::cjGetStaticCls(); \
		cjBSVec3PInterpolator::cjGetStaticCls(); \
		cjBSTMInterpolator::cjGetStaticCls(); \
		cjNode::cjGetStaticCls(); \
		cjRefObjNameHash::cjGetStaticCls(); \
		cjRefObj::cjGetStaticCls(); \
		cjSceneNode::cjGetStaticCls(); \
		cjSceneProp::cjGetStaticCls(); \
		cjBSplineBasisF3Dat::cjGetStaticCls(); \
		cjBSplineDat::cjGetStaticCls(); \


#endif // end of #ifdef CJDLL

#endif
