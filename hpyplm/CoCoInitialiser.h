#ifndef COCOINITIALISER_H_
#define COCOINITIALISER_H_

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include "DefaultPatternModelOptions.h"
#include "ProgramOptions.h"

struct CoCoInitialiser
{   
    ClassEncoder classEncoder;
    ClassDecoder classDecoder;
    IndexedCorpus* indexedCorpus;
    PatternModel<uint32_t> trainPatternModel;
    
    ProgramOptions& po;
    PatternModelOptions& pmo;
    QueryProgramOptions* qpo;
  
    ~CoCoInitialiser() 
    {
        delete indexedCorpus;
    }

    /*
        Needs: po.getInputClassFileName, po.getInputCorpusFileName
     */
    CoCoInitialiser(ProgramOptions& _po, PatternModelOptions& _pmo, bool _trainPatternModel = true, bool onlyClassEncoder = false) : po(_po), pmo(_pmo)
    {  
        std::cout << "Entering CCI for PO" << std::endl;

        classEncoder.load(po.trainClassFileName);
      std::cout << "Done loading class encoder" << std::endl;
    
       if(!onlyClassEncoder)
       {
          classDecoder.load(po.trainClassFileName);
          std::cout << "Done loading class decoder (" << po.trainClassFileName << ")" << std::endl;
          
          indexedCorpus = new IndexedCorpus(po.trainCorpusFileName);
          std::cout << "Done loading indexed corpus (" << po.trainCorpusFileName << ")" << std::endl;
          
          if(_trainPatternModel) trainThePatternModel();
      }
    }

    CoCoInitialiser(QueryProgramOptions& _qpo, PatternModelOptions& _pmo, bool _extendEncoding = true, bool _trainPatternModel = true, bool onlyClassEncoder = false) : CoCoInitialiser((ProgramOptions&) _qpo, _pmo, _trainPatternModel, onlyClassEncoder)
    {
        std::cout << "Entering CCI for QPO" << std::endl;
        qpo = &_qpo; 

        if(_extendEncoding)
        {
           extendEncoding(_qpo.testInputFiles);
           }
    }

    CoCoInitialiser(TrainProgramOptions& _tpo, PatternModelOptions& _pmo, bool computeStats = true, bool reportStats = true) : po(_tpo), pmo(_pmo)
    {
        if(_tpo.clo.loadTrainVocabulary.empty())
        {
            classEncoder.build(_tpo.trainInputFiles, true);
            classEncoder.save(_tpo.trainClassFileName);
        } else
        {
            classEncoder.load(_tpo.clo.loadTrainVocabulary);
        }

        if(_tpo.clo.loadTrainCorpus.empty())
        {
            for(auto i : _tpo.trainInputFiles)
            {
                classEncoder.encodefile(i, _tpo.trainCorpusFileName, 0, 0, 1, 0);
            }
            classDecoder.load(_tpo.trainClassFileName);
        } else
        {
           // do nothing 
        }

        indexedCorpus = new IndexedCorpus(_tpo.trainCorpusFileName);

        trainPatternModel = PatternModel<uint32_t>(indexedCorpus);

        if(_tpo.extendModel.empty())
        {
            if(_tpo.clo.loadTrainPatternModel.empty())
            {
                trainPatternModel.train(_tpo.trainCorpusFileName, _pmo);
                trainPatternModel.write(_tpo.trainPatternModelFileName);
            } else
            {
                trainPatternModel.load(_tpo.trainPatternModelFileName, _pmo);
            }
        } else
        {
            if(_tpo.clo.loadTrainPatternModel.empty())
            {
                trainPatternModel.load(_tpo.extendModel, _pmo);
                trainPatternModel.train(_tpo.clo.loadTrainCorpus, _pmo, nullptr, true, 1, true);
                trainPatternModel.write(_tpo.trainPatternModelFileName);
            } else
            {   
                trainPatternModel.load(_tpo.trainPatternModelFileName, _pmo);
            }
        }

        if(computeStats)
        {
            trainPatternModel.computestats();
            trainPatternModel.computecoveragestats();
        }

        if(reportStats)
        {
            trainPatternModel.report(&std::cerr);
        }
    }

    void trainThePatternModel()
    {
       trainPatternModel = PatternModel<uint32_t>(po.trainPatternModelFileName, pmo, nullptr, indexedCorpus);
    }

    void extendEncoding(std::vector<std::string> inputFiles)
    {
        if(!qpo) { std::cerr << "FIX YOUR POINTERS" << std::endl; }

        for(auto i : inputFiles)
        {
            classEncoder.encodefile(i, qpo->generalOutputCorpusFileName, 1 ,1, 0, 1);
        }
        std::cout << "Done extending the class encoder" << std::endl;
    
        classEncoder.save(qpo->generalOutputClassFileName);
        std::cout << "Done saving the class encoder" << std::endl;

        classDecoder.load(qpo->generalOutputClassFileName);
        std::cout << "Done loading the class decoder" << std::endl;
    }

    void printStats(int order = 4)
    {
        std::cout << "Pattern model stats" << std::endl
                  << indexedCorpus->sentences() << " sentences" << std::endl
                  << trainPatternModel.totalwordtypesingroup(0, 0) << " word types" << std::endl
                  << trainPatternModel.totalpatternsingroup(0,order) << " pattern types" << std::endl
                  << trainPatternModel.totaltokensingroup(0, 1) << " word tokens" << std::endl;
    }

};


#endif
