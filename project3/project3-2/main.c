#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

struct param_type {
    int left;
    int right;
};

void *sort(void *);

void *merge(void *);

int array[100000];

int main() {
    int n;
    pthread_t tid;
    struct param_type param;

    scanf("%d", &n);
    for (int i = 0; i < n; ++i) {
        scanf("%d", array + i);
    }

    param.left = 0;
    param.right = n - 1;

    pthread_create(&tid, NULL, sort, &param);
    pthread_join(tid, NULL);

    for (int j = 0; j < n; ++j) {
        printf("%d ", array[j]);
    }
    printf("\n");

    return 0;
}


void *sort(void *param) {
    struct param_type *ptr = (struct param_type *) param;
    int left = ptr->left;
    int right = ptr->right;
    int mid = (left + right) / 2;
    pthread_t tid_left = 0, tid_right = 0, tid_merge;
    struct param_type param_left, param_right;

    if (mid > left) {
        param_left.left = left;
        param_left.right = mid;
        pthread_create(&tid_left, NULL, sort, (void *) &param_left);
    }
    if (mid + 1 < right) {
        param_right.left = mid + 1;
        param_right.right = right;
        pthread_create(&tid_right, NULL, sort, (void *) &param_right);
    }

    if (tid_left) {
        pthread_join(tid_left, NULL);
    }
    if (tid_right) {
        pthread_join(tid_right, NULL);
    }

    pthread_create(&tid_merge, NULL, merge, param);
    pthread_join(tid_merge, NULL);
    return NULL;
}

void *merge(void *param) {
    struct param_type *ptr = (struct param_type *) param;
    int left = ptr->left;
    int right = ptr->right;
    int mid = (left + right) / 2;
    int i = left, j = mid + 1, k = 0;

    int temp[right - left + 1];
    while (i <= mid && j <= right) {
        if (array[i] < array[j]) {
            temp[k++] = array[i++];
        } else {
            temp[k++] = array[j++];
        }
    }

    for (int l = i; l <= mid; ++l) {
        temp[k++] = array[l];
    }
    for (int l = j; l <= right; ++l) {
        temp[k++] = array[l];
    }

    for (int l = left; l <= right; ++l) {
        array[l] = temp[l - left];
    }

    return NULL;
}
