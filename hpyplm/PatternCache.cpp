/*
 * PatternCache.cpp
 *
 *  Created on: Aug 31, 2016
 *      Author: louis
 */

#include "PatternCache.h"
#include "PLNCache.h"

#include "ContextValues.h"
#include "ContextCounts.h"

Pattern& PatternCache::getPattern()
{
	return pattern;
}

double PatternCache::getWeight()
{
	if(!computed)
		compute();
	return weight_;
}

double PatternCache::getProb()
{
	if(!computed)
		compute();
	return prob_;
}

double P_XXXX::compute()
{
	if(!computed)
	{
		weight_ = parent->cv->get(pattern);
//		prob_ = probLimitedNaiveHelper(focus, context, pattern, 1.0, 1.0, parent->cc, nullptr);

		computed = true;
	}
	return prob_;
}


double P_XXXD::compute()
{
	if(!computed)
	{
		if(pc->get(pattern, nullptr))
		{
//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		} else
		{
			p0_ = parent->xxxx->getWeight() * parent->xxxx->getProb();
			backoff_ = 1.0;

//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		}
		computed = true;
	}
	return prob_;
}


double P_XXCD::compute()
{
	if(!computed)
	{
		weight_ = cc->get(pattern);
		backoff_ = 1.0;
		prob_ = 0.0;
		p0_ = 1.0;
		if(pc->get(pattern, nullptr))
		{
			backoff_ = 0.0;
//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		} else
		{
			backoff_ = 1.0;
			p0_ = parent->xxxd->getWeight() * parent->xxxd->getProb();

//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		}
		computed = true;
	}
	return prob_;
}

double P_XBXD::compute()
{
	if(!computed)
	{
		weight_ = cc->get(pattern);
		backoff_ = 1.0;
		prob_ = 0.0;
		p0_ = 1.0;
		if(pc->get(pattern, nullptr))
		{
			backoff_ = 0.0;
//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		} else
		{
			backoff_ = 1.0;
			p0_ = parent->xxxd->getWeight() * parent->xxxd->getProb();

//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		}
		computed = true;
	}
	return prob_;
}

double P_AXXD::compute()
{
	if(!computed)
	{
		weight_ = cc->get(pattern);
		backoff_ = 1.0;
		prob_ = 0.0;
		p0_ = 1.0;
		if(pc->get(pattern, nullptr))
		{
			backoff_ = 0.0;
//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		} else
		{
			backoff_ = 1.0;
			p0_ = parent->xxxd->getWeight() * parent->xxxd->getProb();

//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		}
		computed = true;
	}
	return prob_;
}

double P_XBCD::compute()
{
	if(!computed)
	{
		weight_ = cc->get(pattern);
		backoff_ = 1.0;
		prob_ = 0.0;
		p0_ = 1.0;
		if(pc->get(pattern, nullptr))
		{
			backoff_ = 0.0;
//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		} else
		{
			backoff_ = 1.0;
			p0_ = parent->xxcd->getWeight() * parent->xxcd->getProb() + parent->xbxd->getWeight() * parent->xbxd->getProb();

//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		}
		computed = true;

	}
	return prob_;
}

double P_AXCD::compute()
{
	if(!computed)
	{
		weight_ = cc->get(pattern);
		backoff_ = 1.0;
		prob_ = 0.0;
		p0_ = 1.0;
		if(pc->get(pattern, nullptr))
		{
			backoff_ = 0.0;
//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		} else
		{
			backoff_ = 1.0;
			p0_ = parent->axxd->getWeight() * parent->axxd->getProb() + parent->xxcd->getWeight() * parent->xxcd->getProb();

//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		}

		computed = true;
	}
	return prob_;
}

double P_ABXD::compute()
{
	if(!computed)
	{
		weight_ = cc->get(pattern);
		backoff_ = 1.0;
		prob_ = 0.0;
		p0_ = 1.0;
		if(pc->get(pattern, nullptr))
		{
			backoff_ = 0.0;
//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		} else
		{
			backoff_ = 1.0;
			p0_ = parent->xbxd->getWeight() * parent->xbxd->getProb() + parent->axxd->getWeight() * parent->axxd->getProb();

//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		}
		computed = true;
	}
	return prob_;
}

double P_ABCD::compute()
{
	std::cout << "Computing for: ABCD" << std::endl;

	if(!computed)
	{
//		weight_ = contextValues->get(context+focus);
		backoff_ = 1.0;
		prob_ = 0.0;
		p0_ = 1.0;

		if(pc->get(pattern, nullptr))
		{

//			auto it = p.find(pattern.reverse());
//			if(it != p.end())
//			{
////				return it->second.probNaive(context, focus,  1.0, delta, 0.0);
//			}
		} else
		{
			backoff_ = 1.0;
			p0_ = parent->xbcd->getWeight() * parent->xbcd->getProb() + parent->axcd->getWeight() * parent->axcd->getProb() + parent->abxd->getWeight() * parent->abxd->getProb();

//			prob_ = probLimitedNaiveHelper(focus, context, pattern, p0_, backoff_, cc, nullptr);
		}
		computed = true;
	}
	return prob_;
}


