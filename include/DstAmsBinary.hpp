#ifndef _DSTAMSBINARY__HPP_
#define _DSTAMSBINARY__HPP_

#include <string>
#include <vector>
#include <iostream>
#include <functional>

#include "Loop.hpp"
#include "rootUtils.hpp"
#include "Stack.hpp"


template <typename T> std::string getType();
template <> std::string getType<float>();
template <> std::string getType<unsigned long long>();
template <> std::string getType<double>();
template <> std::string getType<unsigned int>();
template <> std::string getType<int>();

struct ContainerBase{
    ContainerBase(std::string _name) : name(_name) {}
    std::string name;
    virtual void allocArray(int i) = 0;
    virtual void save(const std::string & outputFileName, int chunkNumber, int chunkStepNumber) = 0;
    virtual void assign(int i) = 0;
    virtual size_t getSize() = 0;
    virtual void writeMetaData(std::ostream & os) = 0;
};



template <typename T, int SIZE = 0 > struct Container : public ContainerBase{
    Container<T,SIZE>(std::string _name, std::function<T()> _f): ContainerBase(_name), f(_f){

    }

    size_t getSize() override {
        return sizeof(T);
    }

    void writeMetaData(std::ostream & os) override {
        os << name << " " << sizeof(T) << " " << getType<T>() << std::endl;
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
};

// Partial specialization for vector<float>
template <int SIZE> struct Container<std::vector<float>, SIZE> : public ContainerBase{
    Container<std::vector<float>,SIZE>(std::string _name, std::function<std::vector<float>() > _f): ContainerBase(_name ), f(_f){
        
    }

    size_t getSize() override {
        return sizeof(float)*SIZE;
    }

    void writeMetaData(std::ostream & os) override {
        for(int i = 0; i < SIZE; i++){
            os << Form("%s_%i",name.c_str(),i) << " " << sizeof(float) << " " << getType<float>() << std::endl;
        }
    }


    void allocArray(int size) override {
        for(int i = 0; i < SIZE; i++){
            var[i] = new float[size];
        }
    }

    std::function<std::vector<float> ()> f;
    float *var[SIZE];

    void assign(int stepNum) override {
        std::vector<float> res = f();
        for(int i = 0; i < SIZE; i++){
            var[i][stepNum] = res[i];
        }
    }
    
    void save(const std::string & outputFileName, int chunkNumber, int chunkStepNumber) override {
        for(int i = 0; i < SIZE; i++){
            std::cout << "outputFileName : " << outputFileName << std::endl;
            std::stringstream fname;
            fname << outputFileName <<"/" << name << Form("_%i_chunk",i) << chunkNumber << ".bin";
            std::cout << "fname.str() : " << fname.str() << std::endl;
            std::ofstream myfile( fname.str(), std::ios::out | std::ios::binary);

            // myfile.write((char*)&chunkStepNumber, sizeof(int));
            myfile.write((char*)var[i], sizeof(float)*chunkStepNumber);
            myfile.close();
        }
    }
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


