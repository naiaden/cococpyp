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

#include "CoCoInitialiser.h"
#include "Debug.h"
#include "LimitedTracker.h"

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
	registerHit();

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
		Debug::getInstance() << DebugLevel::ALL << "H: returning p0: " << p0 << "\n";
		return p0;
	}
}

double P_XXXX::compute(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	if(!computed)
	{
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     A priori probability not computed. \n";

		//LimitedInformation li = parent->lc->get(context);
		prob_ = 1.0 / parent->lc->numberOfFocusWords;//(li.nobackoff + li.backoff);

		weight_ = 1.0;//parent->cv->get(pattern);
		computed = true;
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";
	} else
	{
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     A priori probability already computed.\n";
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";
	}
	return prob_;
}

double P_XXXD::compute(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	if(!computed)
	{
		if(parent->pc->get(pattern, nullptr))
		{
			Debug::getInstance() << DebugLevel::SUBPATTERN << "    Compute unigram prob and stop backoff.\n";

			LimitedInformation li =  parent->lc->get(context);
			p0_ = (1.0 - li.P) / li.nobackoff; //////////// HIER WORDT SUPERHARD 0 door 0 GEDEELD FIX ME
			if(!std::isnormal(p0_))
			{
				Debug::getInstance() << DebugLevel::SUBPATTERN << "p0_ is not normal, resetting to 0\n";
						p0_ = 0.0;
			}


			Debug::getInstance() << DebugLevel::SUBPATTERN << "        liP:" << li.P << " liN:" << li.nobackoff << " liB:" << li.backoff << " p0:" << p0_ << "\n";

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			Debug::getInstance() << DebugLevel::SUBPATTERN << "    Compute unigram prob and continue backoff.\n";

			p0_ = /*parent->xxxx->getWeight(p) * */parent->xxxx->getProb(p);

			prob_ = helper(p, p0_, 1.0);
		}
		weight_ = parent->cv->get(pattern);
		computed = true;
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";
	} else
	{
		Debug::getInstance() << DebugLevel::SUBPATTERN << "    Unigram probability already computed.\n";
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";

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
			Debug::getInstance() << DebugLevel::SUBPATTERN << "   Compute bigram prob and stop backoff.\n";

			LimitedInformation li =  parent->lc->get(context);
			p0_ = (1.0 - li.P) / li.nobackoff;

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			Debug::getInstance() << DebugLevel::SUBPATTERN << "   Compute bigram prob and continue backoff.\n";
			p0_ = /*parent->xxxd->getWeight(p) * */parent->xxxd->getProb(p);

			prob_ = helper(p, p0_, 1.0);
		}
		weight_ = parent->cv->get(pattern);
		computed = true;
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";
	} else
	{
		Debug::getInstance() << DebugLevel::SUBPATTERN << "   Bigram probability already computed.\n";
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";

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
			Debug::getInstance() << DebugLevel::SUBPATTERN << "  Compute xbxd prob and stop backoff.\n";

			LimitedInformation li =  parent->lc->get(context);
			p0_ = (1.0 - li.P) / li.nobackoff;
			if(!std::isnormal(p0_))
			{
				p0_ = CoCoInitialiser::epsilon;
			}

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			Debug::getInstance() << DebugLevel::SUBPATTERN << "  Compute xbxd prob and continue backoff.\n";
			p0_ = /*parent->xxxd->getWeight(p) * */parent->xxxd->getProb(p);

			prob_ = helper(p, p0_, 1.0);
		}
		weight_ = parent->cv->get(pattern);
		computed = true;
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";
	} else
	{
		Debug::getInstance() << DebugLevel::SUBPATTERN << "  xbxd probability already computed.\n";
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";

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
			Debug::getInstance() << DebugLevel::SUBPATTERN << " Compute axxd prob and stop backoff.\n";

			LimitedInformation li =  parent->lc->get(context);
			p0_ = (1.0 - li.P) / li.nobackoff;
			if(!std::isnormal(p0_))
			{
				p0_ = CoCoInitialiser::epsilon;
			}

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			Debug::getInstance() << DebugLevel::SUBPATTERN << " Compute axxd prob and continue backoff.\n";
			p0_ = /*parent->xxxd->getWeight(p) * */parent->xxxd->getProb(p);

			prob_ = helper(p, p0_, 1.0);
		}
		weight_ = parent->cv->get(pattern);
		computed = true;
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";
	} else
	{
		Debug::getInstance() << DebugLevel::SUBPATTERN << " axxd probability already computed.\n";
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";

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
			Debug::getInstance() << DebugLevel::SUBPATTERN << "  Compute xbcd prob and stop backoff.\n";

			LimitedInformation li =  parent->lc->get(context);
			p0_ = (1.0 - li.P) / li.nobackoff;
			if(!std::isnormal(p0_))
			{
				p0_ = CoCoInitialiser::epsilon;
			}

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			Debug::getInstance() << DebugLevel::SUBPATTERN << "  Compute xbcd prob and continue backoff.\n";
			p0_ = parent->xxcd->getWeight(p) * parent->xxcd->getProb(p) + parent->xbxd->getWeight(p) * parent->xbxd->getProb(p);
			p0_ = p0_ / (parent->xxcd->getWeight(p) + parent->xbxd->getWeight(p));
			Debug::getInstance() << DebugLevel::SUBPATTERN << "  -- [" << parent->xxcd->getWeight(p) << "," << parent->xxcd->getProb(p) << "] [" << parent->xbxd->getWeight(p) << "," << parent->xbxd->getProb(p) << "]\n";

			prob_ = helper(p, p0_, 0.0);
		}
		weight_ = parent->cv->get(pattern);
		computed = true;
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";
	} else
	{
		Debug::getInstance() << DebugLevel::SUBPATTERN << "  xbcd probability already computed.\n";
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";

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
			Debug::getInstance() << DebugLevel::SUBPATTERN << " Compute axcd prob and stop backoff.\n";

			LimitedInformation li =  parent->lc->get(context);
			p0_ = (1.0 - li.P) / li.nobackoff;
			if(!std::isnormal(p0_))
			{
				p0_ = CoCoInitialiser::epsilon;
			}

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			Debug::getInstance() << DebugLevel::SUBPATTERN << " Compute axcd prob and continue backoff.\n";
			p0_ = parent->axxd->getWeight(p) * parent->axxd->getProb(p) + parent->xxcd->getWeight(p) * parent->xxcd->getProb(p);
			p0_ = p0_ / (parent->axxd->getWeight(p) + parent->xxcd->getWeight(p));
			Debug::getInstance() << DebugLevel::SUBPATTERN << "  -- [" << parent->axxd->getWeight(p) << "," << parent->axxd->getProb(p) << "] [" << parent->xxcd->getWeight(p) << "," << parent->xxcd->getProb(p) << "]\n";

			prob_ = helper(p, p0_, 0.0);
		}

		weight_ = parent->cv->get(pattern);
		computed = true;
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";
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
			Debug::getInstance() << DebugLevel::SUBPATTERN << " Compute abxd prob and stop backoff.\n";

			LimitedInformation li =  parent->lc->get(context);
			p0_ = (1.0 - li.P) / li.nobackoff;
			if(!std::isnormal(p0_))
			{
				p0_ = CoCoInitialiser::epsilon;
			}

			prob_ = helper(p, p0_, 0.0);
		} else
		{
			Debug::getInstance() << DebugLevel::SUBPATTERN << " Compute abxd prob and continue backoff.\n";
			p0_ = parent->xbxd->getWeight(p) * parent->xbxd->getProb(p) + parent->axxd->getWeight(p) * parent->axxd->getProb(p);
			p0_ = p0_ / (parent->xbxd->getWeight(p) + parent->axxd->getWeight(p));
			Debug::getInstance() << DebugLevel::SUBPATTERN << "  -- [" << parent->xbxd->getWeight(p) << "," << parent->xbxd->getProb(p) << "] [" << parent->axxd->getWeight(p) << "," << parent->axxd->getProb(p) << "]\n";

			prob_ = helper(p, p0_, 0.0);
		}
		weight_ = parent->cv->get(pattern);
		computed = true;
		Debug::getInstance() << DebugLevel::SUBPATTERN << "     --> P" << prob_ << "\tW" << weight_ << "\n";
	}
	return prob_;
}

double P_ABCD::compute(const std::unordered_map<Pattern, cpyp::crp<Pattern>>& p)
{
	Debug::getInstance() << DebugLevel::SUBPATTERN << "ABCD" << ((!computed) ? " not computed yet" : " already computed") << "\n";
	if(!computed)
	{
		prob_ = 0.0;
		p0_ = 1.0;

		if(parent->pc->get(pattern, nullptr))
		{
			Debug::getInstance() << DebugLevel::SUBPATTERN << " Compute abcd prob and stop backoff.\n";

			LimitedInformation li =  parent->lc->get(context);
			p0_ = (1.0 - li.P) / li.nobackoff;
			if(!std::isnormal(p0_))
			{
				p0_ = CoCoInitialiser::epsilon;
			}

			prob_ = helper(p, p0_, 0.0);

		} else
		{
			Debug::getInstance() << DebugLevel::SUBPATTERN << " Compute abcd prob and continue backoff.\n";
			double p1 = parent->xbcd->getWeight(p) * parent->xbcd->getProb(p);
			double p2 = parent->axcd->getWeight(p) * parent->axcd->getProb(p);
			double p3 = parent->abxd->getWeight(p) * parent->abxd->getProb(p);

			p0_ = p1 + p2 + p3;
			p0_ = p0_ / (parent->xbcd->getWeight(p) + parent->axcd->getWeight(p) + parent->abxd->getWeight(p));
			Debug::getInstance() << DebugLevel::SUBPATTERN << "  -- [" << parent->xbcd->getWeight(p) << "," << parent->xbcd->getProb(p) << "] == " << p1 << " [" << parent->axcd->getWeight(p) << "," << parent->axcd->getProb(p) << "] == " << p2 << " [" << parent->abxd->getWeight(p) << "," << parent->abxd->getProb(p) << "] == " << p3 << "\n";

			prob_ = helper(p, p0_, 0.0);
		}
		computed = true;
	}

	LimitedTracker::getInstance().registerHit(PatternPattern::abcd);

	Debug::getInstance() << DebugLevel::SUBPATTERN << "AABBCCDD\t --> P" << prob_ << "\tW" << weight_ << "\n";


	if(prob_ > 0.9999)
	{
		Debug::getInstance() << DebugLevel::ALL << "PROBALITY > 0.9999 NORMALISED TO 1-0.0000000000000001\n";
//		Debug::getInstance() << DebugLevel::ALL << "PROBALITY > 0.9999 NORMALISED TO 1-" << CoCoInitialiser::epsilon << "\n";
		return 1-CoCoInitialiser::epsilon; //
	}

	if(prob_ < 0.0000000001)
	{
		Debug::getInstance() << DebugLevel::ALL << "PROBALITY 0.0000000001 NORMALISED TO 0.0000000000000001\n";
//		Debug::getInstance() << DebugLevel::ALL << "PROBALITY 0.0000000001 NORMALISED TO " << CoCoInitialiser::epsilon << "\n";
		return CoCoInitialiser::epsilon; //
	}
	return prob_;
}

void P_XXXX::registerHit() { LimitedTracker::getInstance().registerHit(PatternPattern::xxxx); }
void P_XXXD::registerHit() { LimitedTracker::getInstance().registerHit(PatternPattern::xxxd); }
void P_XXCD::registerHit() { LimitedTracker::getInstance().registerHit(PatternPattern::xxcd); }
void P_XBXD::registerHit() { LimitedTracker::getInstance().registerHit(PatternPattern::xbxd); }
void P_XBCD::registerHit() { LimitedTracker::getInstance().registerHit(PatternPattern::xbcd); }
void P_AXXD::registerHit() { LimitedTracker::getInstance().registerHit(PatternPattern::axxd); }
void P_AXCD::registerHit() { LimitedTracker::getInstance().registerHit(PatternPattern::axcd); }
void P_ABXD::registerHit() { LimitedTracker::getInstance().registerHit(PatternPattern::abxd); }
void P_ABCD::registerHit() { LimitedTracker::getInstance().registerHit(PatternPattern::abcd); }
