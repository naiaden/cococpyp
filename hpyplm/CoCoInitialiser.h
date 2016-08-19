#ifndef COCOINITIALISER_H_
#define COCOINITIALISER_H_

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include "DefaultPatternModelOptions.h"
#include "ProgramOptions.h"
#include "utils.h"


struct CoCoInitialiser
{   
    ClassEncoder classEncoder;
    ClassDecoder classDecoder;
    IndexedCorpus* indexedCorpus;
    PatternModel<uint32_t> trainPatternModel;
    
    ProgramOptions* po;
    PatternModelOptions& pmo;
  
    static constexpr double epsilon = 0.0000000000000001;

    ~CoCoInitialiser() 
    {
        delete indexedCorpus;
    }

    CoCoInitialiser(PatternModelOptions& _pmo) : pmo(_pmo)
    {

    }

    void initialise(bool _trainPatternModel = true, bool onlyClassEncoder = false)
//    CoCoInitialiser(ProgramOptions& _po, PatternModelOptions& _pmo, bool _trainPatternModel = true, bool onlyClassEncoder = false) : po(_po), pmo(_pmo)
    {  
        std::cout << "Entering CCI for PO" << std::endl;

        classEncoder.load(po->trainClassFileName);
      std::cout << "Done loading class encoder" << std::endl;
    
       if(!onlyClassEncoder)
       {
          classDecoder.load(po->trainClassFileName);
          std::cout << "Done loading class decoder (" << po->trainClassFileName << ")" << std::endl;
          
          indexedCorpus = new IndexedCorpus(po->trainCorpusFileName);
          std::cout << "Done loading indexed corpus (" << po->trainCorpusFileName << ")" << std::endl;
          
          if(_trainPatternModel) trainThePatternModel();
      }
    }

    void trainThePatternModel()
    {
       trainPatternModel = PatternModel<uint32_t>(po->trainPatternModelFileName, pmo, nullptr, indexedCorpus);
    }


    void printStats(my_ostream* mout, int order = 4)
    {
        *mout << "Pattern model stats\n" 
              << indexedCorpus->sentences() << " sentences\n"
              << trainPatternModel.totalwordtypesingroup(0, 0) << " word types\n"
              << trainPatternModel.totalpatternsingroup(0,order) << " " << order << "-gram pattern types" << std::endl;
                  ;//<< trainPatternModel.totaltokensingroup(0, 1) << " word tokens" << std::endl;
    }

};

struct AnalysisCoCoInitialiser : public CoCoInitialiser
{
	AnalysisCoCoInitialiser(AnalysisProgramOptions& _spo, PatternModelOptions& _pmo, bool _extendEncoding = true, bool _trainPatternModel = true, bool onlyClassEncoder = false)
        : CoCoInitialiser(_pmo)
    {
        std::cout << "Entering ACCI" << std::endl;
        po = &_spo;

        initialise(_trainPatternModel, onlyClassEncoder);

    }


};

struct SNCBWCoCoInitialiser : public CoCoInitialiser
{
    SNCBWCoCoInitialiser(SNCBWProgramOptions& _spo, PatternModelOptions& _pmo, bool _extendEncoding = true, bool _trainPatternModel = true, bool onlyClassEncoder = false)
        : CoCoInitialiser(_pmo)
    {
        std::cout << "Entering SNCBWCCI" << std::endl;
        po = &_spo;

        initialise(_trainPatternModel, onlyClassEncoder);

        if(_extendEncoding)
        {
            extendEncoding(_spo.testInputFiles);
        }
    }
    
    void extendEncoding(std::vector<std::string> inputFiles)
    {
        //if(!qpo) { std::cerr << "FIX YOUR POINTERS" << std::endl; }

        std::string gOCFN = dynamic_cast<SNCBWProgramOptions*>(po)->generalOutputCorpusFileName;

        for(auto i : inputFiles)
        {
            classEncoder.encodefile(i, gOCFN, 1 ,1, 0, 1);
        }
        std::cout << "Done extending the class encoder" << std::endl;
    
        classEncoder.save(gOCFN);
        std::cout << "Done saving the class encoder to "
                  << gOCFN 
                  << std::endl;

        classDecoder.load(gOCFN);
        std::cout << "Done loading the class decoder to " 
                  << gOCFN
                  << std::endl;
    }
};

struct TrainCoCoInitialiser : public CoCoInitialiser
{
    TrainCoCoInitialiser(TrainProgramOptions& _tpo, PatternModelOptions& _pmo, bool computeStats = true, bool reportStats = true) : CoCoInitialiser(_pmo)
    {
        std::cout << "Entering TCCI" << std::endl;
        po = &_tpo;

        
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
                trainPatternModel.train(_tpo.clo.loadTrainCorpus, _pmo, nullptr, nullptr, true, 1, true);
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
        	std::cout << "CCI: Computing stats" << std::endl;
            trainPatternModel.computestats();
            trainPatternModel.computecoveragestats();
        }

        if(reportStats)
        {
            trainPatternModel.report(&std::cerr);
        }
    }

};

#endif
