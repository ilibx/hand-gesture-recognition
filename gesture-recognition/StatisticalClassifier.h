/* 
 * File:   StatisticalClassifier.h
 * Author: Alexis
 *
 * Created on 23 novembre 2012, 18:10
 */

#ifndef STATISTICALCLASSIFIER_H
#define	STATISTICALCLASSIFIER_H

#include "opencv2/ml/ml.hpp"
#include "Classifier.h"
#include "TrainableClassifier.h"
#include "TrainableStatModel.h"
#include "math.h"

/**
 * A statistical classifier is a classifier which classifies the segmented
 * hand in 2 (possibly 3) steps:
 * 
 * 1. Computing a caracteristic vector of the hand.
 * 
 * 2. Classifies the caracteristic vector into classes using a statistical
 * model (ie Bayes classifier, 
 * 
 * 3. (Optional) Computes the number of fingers from the classifier result.
 * This may be useful for aggregating classes into one. By default, rounds
 * the result of the classifier.
 */
class StatisticalClassifier : public TrainableClassifier {
public:
    StatisticalClassifier() {
    }
    /**
     * Initializes the statistical classifier from a given statistical model.
     *
     * @param statisticalModel the statistical model used to classify the
     * caracteristic vector.
     */
    StatisticalClassifier(TrainableStatModel *statisticalModel)
        : statisticalModel(statisticalModel)
    {
    }
    /**
     * Returns the opencv statistical model this classifier uses.
     */
    TrainableStatModel *getStatisticalModel() const{
        return this->statisticalModel;
    }
    /**
     * Returns a caracteristic vector for a segmented hand. This caracteristic
     * vector will be used to classify the hand.
     * 
     * @param segmentedHand the hand to classify
     * @return the hand's caracteristic vector for classification
     */
    virtual Mat caracteristicVector(const Mat &segmentedHand) = 0;
    /**
     * Predicts the class a specific caracteristic vector belongs to using
     * previous training data. If numberOfFingersFromClassifierResult is not
     * overridden, this result is simply the number of fingers.
     * 
     * @param caracteristicVector caracteristic vector of the hand to classify.
     * @return the predicted class of the caracteristic vector.
     */
    float predict(const Mat &segmentedHand) {
        Mat caracteristicVector = this->caracteristicVector(segmentedHand);
        cout<<"Caracteristic vector computed"<<endl;
        
        return this->statisticalModel->predict(caracteristicVector);
    }
    /**
     * Returns the number from the result returnes by the opencv statistical
     * model. By default just rounds the result of the classifier.
     * 
     * @param classifierResult result of the classifier.
     * @return the computed number of fingers.
     */
    virtual int numberOfFingersFromClassifierResult(float classifierResult) {
        return round(classifierResult);
    }
    virtual int caracteristicVectorLength(void) = 0;
    int numberOfFingers(Mat &segmentedHand) {
        Mat handCaracteristicVector = this->caracteristicVector(segmentedHand);
        float classifierResult = 
                this->predict(handCaracteristicVector);
        return this->numberOfFingersFromClassifierResult(classifierResult);
    }
    void train(
        const vector<Mat> &segmentedHands,
        const vector<int> &expectedClass) {
        assert(segmentedHands.size() == expectedClass.size());
        Mat trainData(segmentedHands.size(), this->caracteristicVectorLength(), CV_32F);
        cout<<"trainData initialized"<<endl;
        Mat expectedResponses(1, segmentedHands.size(), CV_32F);
        cout<<"responses initialized"<<endl;
    
        for (int i = 0; i < segmentedHands.size(); i++) {
                Mat currentCaracteristicVector = this->caracteristicVector(segmentedHands[i]);
                cout<<"Caracteristic vector for hand "<<i<<" computed: ("<<currentCaracteristicVector.rows<<","<<currentCaracteristicVector.cols<<")"<<endl;
                currentCaracteristicVector.copyTo(trainData.row(i));
                cout<<"Caracteristic vector added to the trainData"<<endl;
                expectedResponses.at<float>(0, i) = expectedClass[i];
                cout<<"Expected response added"<<endl;
        }
    
        this->statisticalModel->train(trainData, expectedResponses);
    }

    void load(const char *filepath) {
        this->statisticalModel->getStatModel()->load(filepath);
    }

    void save(const char *filepath) {
        this->statisticalModel->getStatModel()->save(filepath);
    }

protected:
    TrainableStatModel *statisticalModel;
};

#endif	/* STATISTICALCLASSIFIER_H */
