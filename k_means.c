#include "k_means.h"

extern int k_means_init(group_t *group_head, pixel_color_t **pixel_color_array, int k, int pixel_number, int *pixel_array) {
    srand((unsigned int)time(NULL));
    group_t *tmp_group = NULL;
    pixel_color_t *tmp_pixel;
    int pixel_index;
    int tmp_min_dis;
    int tmp_dis;
    group_t *tmp_min_dis_index;
    group_head->next = NULL;
    group_head->member.next = NULL;
    
    for(int i = 0; i < k; i++) {
        tmp_group = (group_t *)malloc(sizeof(group_t));
        tmp_group->next = group_head->next;
        tmp_group->member.next = NULL;
        pixel_index = rand() % pixel_number;
        tmp_group->center_color[0] = *(pixel_array + pixel_index * 3 + 0);
        tmp_group->center_color[1] = *(pixel_array + pixel_index * 3 + 1);
        tmp_group->center_color[2] = *(pixel_array + pixel_index * 3 + 2);
        group_head->next = tmp_group;
    }

    *pixel_color_array = (pixel_color_t *) malloc(sizeof(pixel_color_t) * pixel_number);
    
    for(int i = 0; i < pixel_number; i++) {
        tmp_min_dis = __INT_MAX__;
        tmp_min_dis_index = NULL;
        tmp_group = group_head->next;
        tmp_pixel = (*pixel_color_array) + i;
        tmp_pixel->color[0] = *(pixel_array + i * 3 + 0);
        tmp_pixel->color[1] = *(pixel_array + i * 3 + 1);
        tmp_pixel->color[2] = *(pixel_array + i * 3 + 2);
        tmp_pixel->position = i;
        while(tmp_group != NULL) {
            tmp_dis = k_means_get_dis(tmp_pixel->color, tmp_group->center_color, 3);
            if(tmp_dis < tmp_min_dis) {
                tmp_min_dis = tmp_dis;
                tmp_min_dis_index = tmp_group;
            }
            tmp_group = tmp_group->next;
        }

        tmp_pixel->grouped = (void *)tmp_min_dis_index;
        tmp_pixel->next = tmp_min_dis_index->member.next;
        tmp_min_dis_index->member.next = tmp_pixel;
    }

    return 0;
}

extern int k_means_run(group_t *group_head, pixel_color_t *pixel_color_array, int pixel_number, int limit, int loop) {
    int now_loop = 0;
    int now_limit = __INT_MAX__;
    pixel_color_t *tmp_pixel_color_array;
    group_t *tmp_group;

    int pixel_index;
    int tmp_min_dis;
    int tmp_dis;
    group_t *tmp_min_dis_index;

    while(1) {
        if(limit >= 0 && now_limit < limit) break;
        if(loop >= 0 && now_loop > loop) break;
        now_loop++;
        now_limit = k_means_get_center(group_head);
        
        if(now_limit == 0) break;
        

        tmp_group = group_head->next;
        while(tmp_group != NULL) {
            tmp_group->member.next = NULL;
            tmp_group = tmp_group->next;
        }

        

        for(int loop_i = 0; loop_i < pixel_number; loop_i++) {
            tmp_pixel_color_array = pixel_color_array + loop_i;
            tmp_group = group_head->next;
            tmp_min_dis = __INT_MAX__;
            tmp_min_dis_index = NULL;
            while(tmp_group != NULL) {
                tmp_dis = k_means_get_dis(tmp_pixel_color_array->color, tmp_group->center_color, 3);
                if(tmp_dis < tmp_min_dis) {
                    tmp_min_dis = tmp_dis;
                    tmp_min_dis_index = tmp_group;
                }
                tmp_group = tmp_group->next;
            }

            tmp_pixel_color_array->grouped = (void *)tmp_min_dis_index;
            tmp_pixel_color_array->next = tmp_min_dis_index->member.next;
            tmp_min_dis_index->member.next = tmp_pixel_color_array;
        }
    }
    return 0;
}

extern int k_means_color(group_t *group_head, pixel_color_t *pixel_color_array, int pixel_number, int *pixel_array) {
    for(int loop_i = 0; loop_i < pixel_number; loop_i++) {
        *(pixel_array + loop_i * 3 + 0) = ((group_t *)((loop_i + pixel_color_array)->grouped))->center_color[0];
        *(pixel_array + loop_i * 3 + 1) = ((group_t *)((loop_i + pixel_color_array)->grouped))->center_color[1];
        *(pixel_array + loop_i * 3 + 2) = ((group_t *)((loop_i + pixel_color_array)->grouped))->center_color[2];
    }
    return 0;
}

extern int k_means_destroy(group_t *group_head, pixel_color_t *pixel_color_array) {
    group_t *tmp_group = group_head->next;
    while(tmp_group !=NULL) {
        tmp_group = tmp_group->next;
        group_head = tmp_group;
        free(group_head);
    }

    free(pixel_color_array);
    return 0;
}

static int k_means_get_center(group_t *group_head) {
    int diff_total, min_dis, old_color[3], tmp_dis;
    group_t *tmp_group = group_head->next;
    pixel_color_t *tmp_pix, *running_pix, *min_pix;

    diff_total = 0;
    while(tmp_group != NULL) {
        memcpy(old_color, tmp_group->center_color, sizeof(int) * 3);
        min_pix = NULL;
        min_dis = __INT_MAX__;
        running_pix = tmp_group->member.next;
        while(running_pix != NULL) {
            tmp_pix = tmp_group->member.next;
            tmp_dis = 0;
            while(tmp_pix != NULL) {
                tmp_dis += k_means_get_dis(tmp_pix->color, running_pix->color, 3);
                tmp_pix = tmp_pix->next;
            }
            if(min_dis > tmp_dis) {
                min_dis = tmp_dis;
                min_pix = running_pix;
            }
            running_pix = running_pix->next;
        }

        memcpy(tmp_group->center_color, min_pix->color, 3);
        diff_total += k_means_get_dis(tmp_group->center_color, old_color, 3);
        tmp_group = tmp_group->next;
    }
    return diff_total;
}

static int k_means_get_dis(int *a, int *b, int len) {
    int diff = 0;
    for(int i = 0; i< len; i++) {
        diff += abs(a[i] - b[i]);
    }
    return diff;
}