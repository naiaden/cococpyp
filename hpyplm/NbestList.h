/*
 * NbestList.h
 *
 *  Created on: Nov 21, 2016
 *      Author: onrust
 */

#ifndef HPYPLM_NBESTLIST_H_
#define HPYPLM_NBESTLIST_H_

#include <string>
#include <vector>

class NBestItem {

public:
	NBestItem(const std::string& aSentence, int cRank, double aScore, double lScore, double words);

	void setRescore(double score);
	void setRank(int rank);

	std::string toString(bool fileFormat = false) const;

	double getAmScore() const;
	double getLmScore() const;
	double getReScore() const;

	bool operator < (const NBestItem& rhs) const
	{
//		return (amScore + lmScore + reScore < rhs.getAmScore() + rhs.getLmScore() + rhs.getReScore());
		return (lmScore < rhs.getLmScore());
	}

private:
	std::string sentence;
	double amScore;
	double lmScore;
	double nrWords;
	int currentRank;
	double reScore;
	int newRank;


};

class FileRenamer {

public:
	std::string rename(const std::string& original, const std::string& outputDir = "");
};

class NbestList {
public:
	NbestList();
	virtual ~NbestList();

	void add(NBestItem* item);

	void determineNewRanks();
	void print(bool fileFormat = false);
	void printToFile(const std::string& originalName, const std::string& outputDirectory);

private:
	std::vector<NBestItem*> items;
	FileRenamer fr;


};

#endif /* HPYPLM_NBESTLIST_H_ */
