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
    IndexedCorpus indexedCorpus;
    PatternModel<uint32_t> trainPatternModel;
    
    ProgramOptions& po;
    PatternModelOptions& pmo;
    QueryProgramOptions* qpo;
  
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
          
          indexedCorpus = IndexedCorpus(po.trainCorpusFileName);
          std::cout << "Done loading indexed corpus (" << po.trainCorpusFileName << ")" << std::endl;
          
          if(_trainPatternModel) trainThePatternModel();
      }
    }

    CoCoInitialiser(QueryProgramOptions& _qpo, PatternModelOptions& _pmo, bool _extendEncoding = true, bool _trainPatternModel = true, bool onlyClassEncoder = false) : CoCoInitialiser((ProgramOptions&) _qpo, _pmo, _trainPatternModel, onlyClassEncoder)
    {
        std::cout << "Entering CCI for QPO" << std::endl;
        qpo = &_qpo; 

     //   if(_extendEncoding)
     //   {
     //      extendEncoding(_qpo.testInputFiles);
     //      }
    }

    void trainThePatternModel()
    {
       trainPatternModel = PatternModel<uint32_t>(po.trainPatternModelFileName, pmo, nullptr, &indexedCorpus);
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

};


#endif
