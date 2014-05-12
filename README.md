This is a fork of cpyp (https://github.com/redpony/cpyp) which I enhanced with Colibri-core (https://github.com/proycon/colibri-core). See hpyplm.cc for the changes.

I chose Colibri because it can also handle skipgrams and flexgrams, one of the ways in which I want to extend cpyp. However, currently there is a nasty segfault generated somewhere in Colibri, and I cannot figure out why this happens.

So for the moment, this project is not under development, until this bug is fixed (either here or in Colibri).

cpyp
====

`cpyp` is a C++ library for nonparametric Bayesian modeling with Pitman-Yor process priors

## Features
- Memory-efficient histogram-based sampling scheme proposed by [Blunsom et al. (2009)](http://www.clg.ox.ac.uk/blunsom/pubs/blunsom-acl09-short.pdf)
- Full range of PYP hyperparameters (0 ≤ discount < 1, strength > -discount, etc.)
- Beta priors on discount hyperparameter
- (Conditional, given discount) Gamma prior on strength hyperparameter
- Tied hyperparameters
- Slice sampling for hyperparameter inference
- “Multifloor” Chinese Restaurant processes to perform inference in graphical Pitman-Yor processes
- Serialization of CRPs using [Boost.Serialization](www.boost.org/libs/serialization) (optional)
- Example implementations
    - Hierarchical Pitman-Yor process language model ([Teh, 2006](http://acl.ldc.upenn.edu/P/P06/P06-1124.pdf))
    - Domain adapting graphical Pitman-Yor process language model ([Wood & Teh, 2009](http://jmlr.csail.mit.edu/proceedings/papers/v5/wood09a/wood09a.pdf))
    - Latent Pitman-Yor allocation topic model (LDA with the D replaced)
    - Unsupervised “Naive Bayes” single-membership clustering, using fast Metropolis-Hastings sampling

## System Requirements
This software requires a C++ compiler that implements the [C++11 standard](http://en.wikipedia.org/wiki/C%2B%2B11), for example [gcc-4.7](http://gcc.gnu.org/) or [Clang-3.1](http://clang.llvm.org/) or something more recent. No other libraries or tools are required.

