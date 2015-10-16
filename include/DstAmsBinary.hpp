#ifndef _DSTAMSBINARY__HPP_
#define _DSTAMSBINARY__HPP_

#include <string>
#include <vector>
#include <iostream>
#include <functional>

#include "Loop.hpp"
#include "rootUtils.hpp"
#include "Stack.hpp"


// template <typename T> std::string getType(){ std::cout << "Unknown specialization" << std::endl; exit(-1); }
// template <> std::string getType<float>(){ return "float"; }
// template <> std::string getType<unsigned long long>(){ return "unsigned long long"; }
// template <> std::string getType<double>(){ return "double"; }
// template <> std::string getType<unsigned int>(){ return "unsigned int"; }
// template <> std::string getType<int>(){ return "int"; }

struct ContainerBase{
    ContainerBase(std::string _name /*, std::string _theType*/) : name(_name) /*, theType(_theType)*/ {}
    // std::string theType;
    std::string name;
    virtual void allocArray(int i) = 0;
    virtual void save(const std::string & outputFileName, int chunkNumber, int chunkStepNumber) = 0;
    virtual void assign(int i) = 0;
    virtual size_t getSize() = 0;
    virtual void writeMetaData(std::ofstream & os) = 0;
};



template <typename T> struct Container : public ContainerBase{
    Container<T>(std::string _name, std::function<T()> _f): ContainerBase(_name /*,getType<T>()*/), f(_f){

    }

    size_t getSize() override {
        return sizeof(T);
    }

    void writeMetaData(std::ofstream & os) override {
        // std::cout << "name : " << name << std::endl;
    }


    void allocArray(int size) override {
        var = new T[size];
    }

    std::function<T()> f;
    T *var;

    void assign(int i) override {
        var[i] = f();
    }
    
    void save(const std::string & outputFileName, int chunkNumber, int chunkStepNumber) override {
        std::cout << "outputFileName : " << outputFileName << std::endl;
        std::stringstream fname;
        fname << outputFileName <<"/" << name << "_chunk" << chunkNumber << ".bin";
        std::cout << "fname.str() : " << fname.str() << std::endl;
        std::ofstream myfile( fname.str(), std::ios::out | std::ios::binary);

        // myfile.write((char*)&chunkStepNumber, sizeof(int));
        myfile.write((char*)var, sizeof(T)*chunkStepNumber);
        myfile.close();
    }



    // void save(){
    //     for(auto it = var.begin(); it != var.end() ;it++){
    //         std::cout << "outputFileName : " << outputFileName << std::endl;
    //         std::stringstream fname;
    //         fname << outputFileName <<"/" << it->first << "_chunk" << chunkNumber << ".bin";
    //         std::cout << "fname.str() : " << fname.str() << std::endl;
    //         std::ofstream myfile( fname.str(), std::ios::out | std::ios::binary);

    //         // myfile.write((char*)&chunkStepNumber, sizeof(int));
    //         myfile.write((char*)&(it->second[0]), sizeof(float)*chunkStepNumber);
    //         myfile.close();
    //     }
    // }
};


class DstAmsBinary : public Loop{
public:
    DstAmsBinary( std::string _data, int _maxRAM ) : Loop(_data),
                                        chunkStepNumber(0),
                                        maxRAM(_maxRAM)
    {}

    virtual ~DstAmsBinary(){}
    virtual void registerVariables() = 0;
    virtual void initPointers() = 0;

    void saveChunk();
    void end();
    bool process();
    void init();
    void saveMetaData();
    int cutEvent();

protected:
    float p;
    float R;
    bool isMC;
    int nChi2Cut;
    unsigned int chunkStepNumber;
    unsigned int chunkSize;
    int nVar;
    int maxRAM;

    std::vector<ContainerBase*> variables;
};

#endif


