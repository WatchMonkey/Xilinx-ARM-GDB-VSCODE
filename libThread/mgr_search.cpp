/*!
 * @file mgr_search.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "mgr_header.h"
#include "mgr_data.h"


bool check_unique(int tag_id)
{
    int cnt = 0x0;
    for(int i = 0x0;i < MGR_THREAD_MAX;++i)
    {
        if(0x0 > get_iden(i)){
            continue;
        }
        if(tag_id != get_iden(i)){
            continue;
        }
        cnt++;
    }
    if(0x1 >= cnt)
        return true;
    return false;
}


bool check_full(int& first)
{
    int jump = false;
    int cnt = 0x0;
    for(int i = 0x0;i < MGR_THREAD_MAX;++i)
    {
        if(0x0 <= get_iden(i)){
            continue;
        }
        cnt++;
        if(!jump){
            first = i;jump = !jump;
        }
        break;
    }
    return !cnt;
}


int find_array(int tag_id)
{
    if(false == check_unique(tag_id)){
        return FIND_MULTY;
    }
    //
    int rt = FIND_NOTHING;
    for(int i = 0x0;i < MGR_THREAD_MAX;++i)
    {
        if(0x0 > get_iden(i)){
            continue;
        }
        if(tag_id != get_iden(i)){
            continue;
        }
        rt = i;
    }
    return rt;
}
