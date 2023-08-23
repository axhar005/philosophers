/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oboucher <oboucher@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 13:14:21 by oboucher          #+#    #+#             */
/*   Updated: 2023/08/23 17:55:51 by oboucher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

t_data	*data(void)
{
	static t_data	dt;

	return (&dt);
}

void msleep(t_time duration, t_philo *philo)
{
    t_time start;

    (void)philo;
    start = get_time();
    if (philo->last_eat < duration + start && !check_one_death() && philo->last_eat > start)
    {
        usleep((philo->last_eat - start) * 1000);
		check_all_dead(philo);
    }
    else
    {
        while (get_time() < duration + start)
			usleep(150);
        // while(true)
        // {
        //     if (get_time() - start >= duration || check_one_death())
        //         break;
        //     else    
        //         usleep(200);
        // }
    }
}

t_time    get_time(void)
{
    static struct timeval    start = {-1, -1};
    struct timeval            real_time;

    if (start.tv_sec == -1 && start.tv_usec == -1)
        gettimeofday(&start, NULL);
    gettimeofday(&real_time, NULL);
    return ((real_time.tv_sec * 1000 - start.tv_sec * 1000)
        + real_time.tv_usec / 1000 - start.tv_usec / 1000);
}

void    print_state(size_t philo_id, char *str)
{
    pthread_mutex_lock(&data()->mutex.print);
    if (!check_one_death())
        printf("%lld %zu %s\n", get_time(), philo_id, str);
    pthread_mutex_unlock(&data()->mutex.print);
}

void    check_eat(t_philo *philo)
{
    if (!check_all_dead(philo))
    {
        // if (!data()->only_the_one)
        // {
            pthread_mutex_lock(&philo->fork);
            print_state(philo->id, FORK);
            pthread_mutex_lock(philo->mate_fork);
            print_state(philo->id, FORK);
            print_state(philo->id, EATING);
            msleep(data()->time_to_eat, philo);
            pthread_mutex_lock(&data()->mutex.last_eat);
            philo->last_eat = get_time();
            pthread_mutex_unlock(&data()->mutex.last_eat);
            pthread_mutex_unlock(&philo->fork);
            pthread_mutex_unlock(philo->mate_fork);
        // }
    }
}

void *routine(void *param)
{
    t_philo *philo;
    
    philo = (t_philo *)param;
    if (philo->id % 2 == 0)
        msleep(10, philo);
    while (!check_one_death())
    {
        print_state(philo->id, THINKING);
        if (!check_one_death())
            check_eat(philo);
        // if (philo->eat_number > 0 && data()->limited_eat)
        //     philo->eat_number--;
        // if (philo->eat_number == 0 && data()->limited_eat)
        //     break ;
        if (!check_one_death())
        {
            print_state(philo->id, SLEEPING);
            msleep(data()->time_to_sleep, philo);
        }
    }
    return(NULL);
}

int main(int ac, char **av)
{
    // (void)av;
    // (void)ac;
    if (ac == 5 || ac == 6)
    {
        if (pars(av))
        {
            init_mutex(INIT);
            init_philo();
            create_philo();
            join_philo();
        }
    }
    else
        printf("Error: bad arguments\n");
    return (0);
}