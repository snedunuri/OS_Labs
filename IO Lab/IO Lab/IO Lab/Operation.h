//
//  Operation.h
//  IO Lab
//
//  Created by  Satya Nedunuri on 4/24/16.
//  Copyright © 2016  Satya Nedunuri. All rights reserved.
//

#ifndef Operation_h
#define Operation_h

struct Operation{
    unsigned int time;
    unsigned int track;
    unsigned int opid;
    unsigned int movement;
    unsigned int submission_time;
    unsigned int issue_time;
    unsigned int completion_time;
    
    inline bool operator< (const Operation& op1) const {
        return track < op1.track;
    }
};

struct order_by_id {
    inline bool operator() (const Operation& op1, const Operation& op2) const {
        return op1.opid < op2.opid;
    }
};

struct less_than {
    inline bool operator() (const Operation& op1, const Operation& op2) const {
        return op1.track < op2.track;
    }
};

#endif /* Operation_h */
