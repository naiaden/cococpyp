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
#include "PatternCounts.h"

#include "LimitedCounts.h"

Pattern& PatternCache::getPattern()
{
	return pattern;
}

double PatternCache::getWeight(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	if(!computed)
		compute(p);
	return weight_;
}

double PatternCache::getProb(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	if(!computed)
		compute(p);
	return prob_;
}

double PatternCache::helper(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p, double p0, double S)
{
	auto it = p.find(context.reverse());
	if(it != p.end())
	{
		return it->second.probNaive(context, focus, p0, S);

	} else
	{
		std::cout << "H: returning p0: " << p0 << std::endl;
		return p0;
	}
}

double P_XXXX::compute(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	if(!computed)
	{
		std::cout << "     A priori probabily not computed. " << std::endl;

		LimitedInformation li = parent->lc->get(context, nullptr);
		prob_ = 1.0 / (li.nobackoff + li.backoff);

		weight_ = parent->cv->get(pattern);
		computed = true;
	} else
	{
		std::cout << "     A priori probabily already computed." << std::endl;
	}
	return prob_;
}

double P_XXXD::compute(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	if(!computed)
	{
		if(parent->pc->get(pattern, nullptr))
		{
			std::cout << "    Compute unigram prob and stop backoff." << std::endl;

			LimitedInformation li =  parent->lc->get(context, nullptr);
			p0_ = (1.0 - li.P) / li.nobackoff;

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			std::cout << "    Compute unigram prob and continue backoff." << std::endl;

			p0_ = /*parent->xxxx->getWeight(p) * */parent->xxxx->getProb(p);

			prob_ = helper(p, p0_, 1.0);
		}
		weight_ = parent->cv->get(pattern);
		computed = true;
	} else
	{
		std::cout << "    Unigram probability already computed." << std::endl;
	}
	return prob_;
}

double P_XXCD::compute(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	if(!computed)
	{
		weight_ = parent->cc->get(pattern);
		prob_ = 0.0;
		p0_ = 1.0;
		if(parent->pc->get(pattern, nullptr))
		{
			std::cout << "   Compute bigram prob and stop backoff." << std::endl;

			LimitedInformation li =  parent->lc->get(context, nullptr);
			p0_ = (1.0 - li.P) / li.nobackoff;

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			std::cout << "   Compute bigram prob and continue backoff." << std::endl;
			p0_ = /*parent->xxxd->getWeight(p) * */parent->xxxd->getProb(p);

			prob_ = helper(p, p0_, 1.0);
		}
		weight_ = parent->cv->get(pattern);
		computed = true;
	} else
	{
		std::cout << "   Bigram probability already computed." << std::endl;
	}
	return prob_;
}

double P_XBXD::compute(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	if(!computed)
	{
		weight_ = parent->cc->get(pattern);
		prob_ = 0.0;
		p0_ = 1.0;
		if(parent->pc->get(pattern, nullptr))
		{
			std::cout << "  Compute xbxd prob and stop backoff." << std::endl;

			LimitedInformation li =  parent->lc->get(context, nullptr);
			p0_ = (1.0 - li.P) / li.nobackoff;
			if(!std::isnormal(p0_))
			{
				p0_ = 0.000000000000000001;
			}

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			std::cout << "  Compute xbxd prob and continue backoff." << std::endl;
			p0_ = /*parent->xxxd->getWeight(p) * */parent->xxxd->getProb(p);

			prob_ = helper(p, p0_, 1.0);
		}
		weight_ = parent->cv->get(pattern);
		computed = true;
	} else
	{
		std::cout << "  xbxd probability already computed." << std::endl;
	}
	return prob_;
}

double P_AXXD::compute(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	if(!computed)
	{
		weight_ = parent->cc->get(pattern);
		prob_ = 0.0;
		p0_ = 1.0;
		if(parent->pc->get(pattern, nullptr))
		{
			std::cout << " Compute axxd prob and stop backoff." << std::endl;

			LimitedInformation li =  parent->lc->get(context, nullptr);
			p0_ = (1.0 - li.P) / li.nobackoff;
			if(!std::isnormal(p0_))
			{
				p0_ = 0.000000000000000001;
			}

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			std::cout << " Compute axxd prob and continue backoff." << std::endl;
			p0_ = /*parent->xxxd->getWeight(p) * */parent->xxxd->getProb(p);

			prob_ = helper(p, p0_, 1.0);
		}
		weight_ = parent->cv->get(pattern);
		computed = true;
	} else
	{
		std::cout << " axxd probability already computed." << std::endl;
	}
	return prob_;
}

double P_XBCD::compute(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	if(!computed)
	{
		weight_ = parent->cc->get(pattern);
		prob_ = 0.0;
		p0_ = 1.0;
		if(parent->pc->get(pattern, nullptr))
		{
			std::cout << "  Compute xbcd prob and stop backoff." << std::endl;

			LimitedInformation li =  parent->lc->get(context, nullptr);
			p0_ = (1.0 - li.P) / li.nobackoff;
			if(!std::isnormal(p0_))
			{
				p0_ = 0.000000000000000001;
			}

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			std::cout << "  Compute xbcd prob and continue backoff." << std::endl;
			p0_ = parent->xxcd->getWeight(p) * parent->xxcd->getProb(p) + parent->xbxd->getWeight(p) * parent->xbxd->getProb(p);
			p0_ = p0_ / (parent->xxcd->getWeight(p) + parent->xbxd->getWeight(p));
			std::cout << "  -- [" << parent->xxcd->getWeight(p) << "," << parent->xxcd->getProb(p) << "] [" << parent->xbxd->getWeight(p) << "," << parent->xbxd->getProb(p) << "]" << std::endl;

			prob_ = helper(p, p0_, 0.0);
		}
		weight_ = parent->cv->get(pattern);
		computed = true;
	} else
	{
		std::cout << "  xbcd probability already computed." << std::endl;
	}
	return prob_;
}

double P_AXCD::compute(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	if(!computed)
	{
		weight_ = parent->cc->get(pattern);
		prob_ = 0.0;
		p0_ = 1.0;
		if(parent->pc->get(pattern, nullptr))
		{
			std::cout << " Compute axcd prob and stop backoff." << std::endl;

			LimitedInformation li =  parent->lc->get(context, nullptr);
			p0_ = (1.0 - li.P) / li.nobackoff;
			if(!std::isnormal(p0_))
			{
				p0_ = 0.000000000000000001;
			}

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			std::cout << " Compute axcd prob and continue backoff." << std::endl;
			p0_ = parent->axxd->getWeight(p) * parent->axxd->getProb(p) + parent->xxcd->getWeight(p) * parent->xxcd->getProb(p);
			p0_ = p0_ / (parent->axxd->getWeight(p) + parent->xxcd->getWeight(p));
			std::cout << "  -- [" << parent->axxd->getWeight(p) << "," << parent->axxd->getProb(p) << "] [" << parent->xxcd->getWeight(p) << "," << parent->xxcd->getProb(p) << "]" << std::endl;

			prob_ = helper(p, p0_, 0.0);
		}

		weight_ = parent->cv->get(pattern);
		computed = true;
	}
	return prob_;
}

double P_ABXD::compute(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	if(!computed)
	{
		weight_ = parent->cc->get(pattern);
		prob_ = 0.0;
		p0_ = 1.0;
		if(parent->pc->get(pattern, nullptr))
		{
			std::cout << " Compute abxd prob and stop backoff." << std::endl;

			LimitedInformation li =  parent->lc->get(context, nullptr);
			p0_ = (1.0 - li.P) / li.nobackoff;
			if(!std::isnormal(p0_))
			{
				p0_ = 0.000000000000000001;
			}

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			std::cout << " Compute abxd prob and continue backoff." << std::endl;
			p0_ = parent->xbxd->getWeight(p) * parent->xbxd->getProb(p) + parent->axxd->getWeight(p) * parent->axxd->getProb(p);
			p0_ = p0_ / (parent->xbxd->getWeight(p) + parent->axxd->getWeight(p));
			std::cout << "  -- [" << parent->xbxd->getWeight(p) << "," << parent->xbxd->getProb(p) << "] [" << parent->axxd->getWeight(p) << "," << parent->axxd->getProb(p) << "]" << std::endl;

			prob_ = helper(p, p0_, 0.0);
		}
		weight_ = parent->cv->get(pattern);
		computed = true;
	}
	return prob_;
}

double P_ABCD::compute(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	std::cout << "." << ((!computed) ? " not computed yet" : "already computed") << std::endl;
	if(!computed)
	{
		prob_ = 0.0;
		p0_ = 1.0;

		if(parent->pc->get(pattern, nullptr))
		{
			std::cout << " Compute abcd prob and stop backoff." << std::endl;

			LimitedInformation li =  parent->lc->get(context, nullptr);
			p0_ = (1.0 - li.P) / li.nobackoff;
			if(!std::isnormal(p0_))
			{
				p0_ = 0.000000000000000001;
			}

			prob_ = helper(p, p0_, 0.0);

		} else
		{
			std::cout << " Compute abcd prob and continue backoff." << std::endl;
			double p1 = parent->xbcd->getWeight(p) * parent->xbcd->getProb(p);
			double p2 = parent->axcd->getWeight(p) * parent->axcd->getProb(p);
			double p3 = parent->abxd->getWeight(p) * parent->abxd->getProb(p);

			p0_ = p1 + p2 + p3;
			p0_ = p0_ / (parent->xbcd->getWeight(p) + parent->axcd->getWeight(p) + parent->abxd->getWeight(p));
			std::cout << "  -- [" << parent->xbcd->getWeight(p) << "," << parent->xbcd->getProb(p) << "] == " << p1 << " [" << parent->axcd->getWeight(p) << "," << parent->axcd->getProb(p) << "] == " << p2 << " [" << parent->abxd->getWeight(p) << "," << parent->abxd->getProb(p) << "] == " << p3 << std::endl;

			prob_ = helper(p, p0_, 0.0);
		}
		computed = true;
	}
	return prob_;
}


