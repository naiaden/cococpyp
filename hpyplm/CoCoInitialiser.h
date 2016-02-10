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
    SNCBWProgramOptions* spo;
  
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

    CoCoInitialiser(SNCBWProgramOptions& _spo, PatternModelOptions& _pmo, bool _extendEncoding = true, bool _trainPatternModel = true, bool onlyClassEncoder = false)
        : CoCoInitialiser((ProgramOptions&) _spo, _pmo, _trainPatternModel, onlyClassEncoder)
    {
        std::cout << "Entering CCI for SNCBWPO" << std::endl;
        spo = &_spo;

        if(_extendEncoding)
        {
            extendEncoding(_spo.testInputFiles);
        }
    }

    CoCoInitialiser(TrainProgramOptions& _tpo, PatternModelOptions& _pmo, bool computeStats = true, bool reportStats = true) : po(_tpo), pmo(_pmo)
    {
        std::string cls;
        
        if(_tpo.clo.loadTrainVocabulary.empty())
        {
            std::cout << "CCI: Training vocabulary from " << std::endl;
            for(auto i : _tpo.trainInputFiles)
            {
                std::cout << "     " << i << std::endl;
            }
            classEncoder.build(_tpo.trainInputFiles, true);
            std::cout << "CCI: Saving to " << _tpo.generalBaseClassFileName << std::endl;
            classEncoder.save(_tpo.generalBaseClassFileName);

            cls = _tpo.generalBaseClassFileName;
        } else
        {
            std::cout << "CCI: Loading vocabulary from " << _tpo.clo.loadTrainVocabulary << std::endl;
            classEncoder.load(_tpo.clo.loadTrainVocabulary);

            cls = _tpo.clo.loadTrainVocabulary;
        }

        std::string dat;

        if(_tpo.clo.loadTrainCorpus.empty())
        {
            std::cout << "CCI: Training corpus from " << std::endl;
            for(auto i : _tpo.trainInputFiles)
            {
                std::cout << "     " << i << std::endl;
            }

            int append = 0;
            for(auto i : _tpo.trainInputFiles)
            {
                classEncoder.encodefile(i, _tpo.generalBaseCorpusFileName, 0, 0, append, 0);
                append = 1;
            }
//            std::cout << "CCI: Loading corpus from " << _tpo.generalBaseClassFileName << std::endl;
//            classDecoder.load(_tpo.generalBaseClassFileName);
            std::cout << "CCI: Loading corpus from " << cls << std::endl;
            classDecoder.load(cls);

            dat = _tpo.generalBaseCorpusFileName;
        } else
        {
            dat = _tpo.trainCorpusFileName;
        }
        std::cout << "CCI: Creating IndexedCorpus from " << dat << std::endl;
        indexedCorpus = new IndexedCorpus(dat);


        std::cout << "CCI: Creating pattern model from indexed corpus" << std::endl;
        trainPatternModel = PatternModel<uint32_t>(indexedCorpus);

        if(_tpo.extendModel.empty())
        {
            if(_tpo.clo.loadTrainPatternModel.empty())
            {
                std::cout << "CCI: Training pattern model from " << dat << std::endl;
                trainPatternModel.train(dat, _pmo);
                std::cout << "CCI: Writing pattern model to " << _tpo.generalBasePatternModelFileName << std::endl;
                trainPatternModel.write(_tpo.generalBasePatternModelFileName);
            } else
            {
                std::cout << "CCI: Load pattern model from " << _tpo.trainPatternModelFileName << std::endl;
                trainPatternModel.load(_tpo.trainPatternModelFileName, _pmo);
            }
        } else
        {
            if(_tpo.clo.loadTrainPatternModel.empty())
            {
                std::cout << "CCI: Load existing model to extend from " << _tpo.extendModel << std::endl;
                trainPatternModel.load(_tpo.extendModel, _pmo);
                std::cout << "CCI: Train the extended model from " << _tpo.clo.loadTrainCorpus << std::endl;
                trainPatternModel.train(_tpo.clo.loadTrainCorpus, _pmo, nullptr, true, 1, true);
                std::cout << "CCI: Write extended pattern model to " << _tpo.generalBasePatternModelFileName << std::endl;
                trainPatternModel.write(_tpo.generalBasePatternModelFileName);
            } else
            {   
                std::cout << "CCI: Load pattern model from " << _tpo.trainPatternModelFileName << std::endl;
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
                  << trainPatternModel.totalpatternsingroup(0,order) << " " << order << "-gram pattern types" << std::endl
                  ;//<< trainPatternModel.totaltokensingroup(0, 1) << " word tokens" << std::endl;
    }

};


#endif
