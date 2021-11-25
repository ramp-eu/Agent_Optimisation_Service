#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

float optimal_agents (float **flowrate, float **distances, int N, float agent_speed, float load_time, float unload_time, float capacity, float availability, float traffic_factor, float operator_efficiency);

int main ()
{

mongoc_database_t *database;
mongoc_client_t *client;
mongoc_collection_t *collection;
mongoc_cursor_t *cursor;
const bson_t *doc;
bson_t *query;
bson_iter_t iter;
const bson_value_t *value;
bson_t *update = NULL;
bson_error_t error;
char buf1[100], buf2[100];
const char *str_val, *str_key;
char str_matrix_cut_val[65535 ], str_matrix_element_val[50];
uint32_t length;
float **distances;
float **flowrate;
int N = 0, i = 0, j = 0, row = 0, col = 0, request_id = 0;
int comma_count = 0;
int num_count = 0;
int str_lenght = 0;
int int_length;
float agent_speed = 0.0, load_time = 0.0, unload_time = 0.0, capacity = 0.0, availability = 0.0, traffic_factor = 0.0, operator_efficiency = 0.0;
float result_optimal_number_of_agents;
int result_optimal_number_of_agents_rounded;
char* sss;

printf("Optimization started...");

// init the connector
mongoc_init ();

// set mongodb location
client = mongoc_client_new("mongodb://localhost:27017/");
if (!client) 
{
  printf("No client connection");
  return EXIT_FAILURE;
}

// set client name, database name and collection name
mongoc_client_set_appname (client, "ncr-client");
database = mongoc_client_get_database (client, "AGENT_OPTIMIZATION");
collection = mongoc_client_get_collection (client, "AGENT_OPTIMIZATION", "AGENTS");

// create empty query
query = bson_new ();

// add to empty query filter only for non calculated data (where result_DONE = 0)
BSON_APPEND_UTF8 (query, "result_DONE", "0");

// find data
cursor = mongoc_collection_find_with_opts (collection, query, NULL, NULL);

// read out data (if are present) - document
while (mongoc_cursor_next (cursor, &doc))
{
   // do iteration(s) inside document
   bson_iter_init(&iter,doc);
   
   // read out data - fields (keys and values) in document
   while(bson_iter_next(&iter))
   {
       //value = bson_iter_value(&iter);
       //printf("output %s : %s\n\n",bson_iter_key(&iter),bson_iter_utf8(&iter, &length));
       
       // read key and key value
       str_key = bson_iter_key(&iter);
       str_val = bson_iter_utf8(&iter, &length);
       
       // if meaningful data are present
       if (str_val!=NULL)
       {
          // if key name found, print the key name and store the values into variables      
          if (strcmp(str_key,"request_id") == 0) { request_id=atoi(str_val); printf("reading request_id (%d)\n",request_id);}
          if (strcmp(str_key,"N") == 0)
          {
            // read the matrix size N (distance and flowrate matrices are NxN size)
            printf("reading N\n");
            // convert str to int
            N=atoi(str_val);
            
            // dynamic allocation of memory for distances matrix
            distances = malloc(N*sizeof(float*));
	    if(distances==NULL)
	    {
	      printf("\nError allocating memory\n");
	      exit(1);
	    }
	    // allocate each row
	    for(i = 0; i < N; i++)
	    {
	      distances[i] = malloc(N*sizeof(float));
	    }
	    if (distances[i-1]==NULL)
	    {
	      printf("\nError allocating memory\n");
	      exit(1);
	    }
	    
	    // dynamic allocation of memory for flowrate matrix
	    flowrate = malloc(N*sizeof(float*));
	    if(flowrate==NULL)
	    {
	      printf("\nError allocating memory\n");
	      exit(1);
	    }
	    // allocate each row
	    for(i = 0; i < N; i++)
	    {
	      flowrate[i] = malloc(N*sizeof(float));
	    }
	    if (flowrate[i-1]==NULL)
	    {
	      printf("\nError allocating memory\n");
	      exit(1);
	    }
          }
          if (strcmp(str_key,"agent_speed") == 0) { printf("reading agent_speed\n"); agent_speed=atof(str_val); }
          if (strcmp(str_key,"agent_load_time") == 0) { printf("reading agent_load_time\n"); load_time=atof(str_val); }
          if (strcmp(str_key,"agent_unload_time") == 0) { printf("reading agent_unload_time\n"); unload_time=atof(str_val); }
          if (strcmp(str_key,"agent_capacity") == 0) { printf("reading agent_capacity\n"); capacity=atof(str_val); }
          if (strcmp(str_key,"agent_availability") == 0) { printf("reading agent_availability\n"); availability=atof(str_val); }
          if (strcmp(str_key,"traffic_factor") == 0) { printf("reading traffic_factor\n"); traffic_factor=atof(str_val); }
          if (strcmp(str_key,"operator_efficiency") == 0) { printf("reading operator_efficiency\n"); operator_efficiency=atof(str_val); }
          if (strcmp(str_key,"distance_matrix") == 0)
          {
            printf("reading distance_matrix\n");
            
            // calculate the length of distance matrix key value (example "10,0,3,0," -> 9 chars) 
            str_lenght = 0;
            while (str_val[str_lenght] != 0)
            {
              str_lenght = str_lenght +1;
            }
                        
            // copy the conts str (read only memory) into str (writeble mamory area)
            for (i = 0; i <= str_lenght; i++)
            {
              str_matrix_cut_val[i] = str_val[i];
            }
            // add NULL (termination / end of string)
            str_matrix_cut_val[i+1] = 0;
            
            
            // number of commas
            comma_count = 0;
            // number of digits
            num_count = 0;
            // column and row varibles
            col = 0;
            row = 0;
            // find commas
            // the encoded matrix starts with for example "10,0,"
            // - digits are found first
            // - after some digits the comma is next
            for (i = 0; i <= str_lenght; i++)
            {
              // if comma (, or ;) found
              if ((str_matrix_cut_val[i] == 44) || (str_matrix_cut_val[i] == 59))
              {
                // put NULL at the end of string 
                // string contains extracted digits
                str_matrix_element_val[num_count] = 0;
                
                // reset the number of digits for the next element
                num_count = 0;
                
                // convert str to float and store into matrix
                distances[row][col] = atof(str_matrix_element_val);
                // new column
                col = col + 1;
                // we use numbering from 0 to N-1 (NxN matrix has N-1xN-1 alocated size)
                if (col == N)
                {
                  // then reset the column and start a new row
                  col = 0;
                  row = row + 1;
                }
              }
              else
              {
                // if digit, store into variable for later conversion to float
                str_matrix_element_val[num_count] = str_matrix_cut_val[i];
                // increase the str length (number of digits)
                num_count = num_count + 1;
              }
            }
          }
          if (strcmp(str_key,"flowrate_matrix") == 0)
          {
            printf("reading distance_matrix\n");
            
            // calculate the length of distance matrix key value (example "10,0,3,0," -> 9 chars) 
            str_lenght = 0;
            while (str_val[str_lenght] != 0)
            {
              str_lenght = str_lenght +1;
            }
                        
            // copy the conts str (read only memory) into str (writeble mamory area)
            for (i = 0; i <= str_lenght; i++)
            {
              str_matrix_cut_val[i] = str_val[i];
            }
            // add NULL (termination / end of string)
            str_matrix_cut_val[i+1] = 0;
            
            
            // number of commas
            comma_count = 0;
            // number of digits
            num_count = 0;
            // column and row varibles
            col = 0;
            row = 0;
            // find commas
            // the encoded matrix starts with for example "10,0,"
            // - digits are found first
            // - after some digits the comma is next
            for (i = 0; i <= str_lenght; i++)
            {
              // if comma (, or ;) found
              if ((str_matrix_cut_val[i] == 44) || (str_matrix_cut_val[i] == 59))
              {
                // put NULL at the end of string 
                // string contains extracted digits
                str_matrix_element_val[num_count] = 0;
                
                // reset the number of digits for the next element
                num_count = 0;
                
                // convert str to float and store into matrix
                flowrate[row][col] = atof(str_matrix_element_val);
                // new column
                col = col + 1;
                // we use numbering from 0 to N-1 (NxN matrix has N-1xN-1 alocated size)
                if (col == N)
                {
                  // then reset the column and start a new row
                  col = 0;
                  row = row + 1;
                }
              }
              else
              {
                // if digit, store into variable for later conversion to float
                str_matrix_element_val[num_count] = str_matrix_cut_val[i];
                // increase the str length (number of digits)
                num_count = num_count + 1;
              }
            }
          }
       }      
   }
   
   // call the optimization
   result_optimal_number_of_agents = optimal_agents(flowrate, distances, N, agent_speed, load_time, unload_time, capacity, availability, traffic_factor, operator_efficiency);
   
   // print the result as float data type
   printf("result from optim. function %f\n", result_optimal_number_of_agents);
   
   // request_id to str
   int_length = snprintf( NULL, 0, "%d", request_id);
   sss = malloc(int_length + 1);
   snprintf(sss, int_length + 1, "%d", request_id );
   // query will be used to filter and update record for request_id
   query = BCON_NEW ("request_id", sss);
   // print the query - for debug
   //printf("query: %s \n",bson_as_canonical_extended_json (query, NULL));
   free(sss);
   
   // optimal number of agentd (float) is converted to str
   // 6 digits from the float are converted to str
   gcvt(result_optimal_number_of_agents, 6, buf1);
   // print the buf1 - for debug
   // printf("result_optimal_number_of_agents (buf1): %s\n",buf1);
   
   // round the optimal number of agents and store into int data type
   result_optimal_number_of_agents_rounded = ceil (result_optimal_number_of_agents);
   // conver it to str
   int_length = snprintf( NULL, 0, "%d", result_optimal_number_of_agents_rounded);
   sss = malloc(int_length + 1);
   snprintf(buf2, int_length + 1, "%d", result_optimal_number_of_agents_rounded);
   free(sss);
   // print the buf2 - for debug
   // printf("result_optimal_number_of_agents_rounded (buf2): %s\n",buf2);
   
   // the update query specification
   // buf1 represents the optimal number of agents converted from float to str
   // buf2 represents the rounded value converted from int to str
   // result_DONE = 1 will set the flag: optimization was performed
   update = BCON_NEW ("$set", "{","result_optimal_number_of_agents", BCON_UTF8(buf1),"result_optimal_number_of_agents_rounded", BCON_UTF8(buf2),"result_DONE", BCON_UTF8("1"),"}");
   // print the query - for debug
   //printf("update: %s \n",bson_as_canonical_extended_json (update, NULL));
   //exit(0);
   
   // execute the update query
   if (!mongoc_collection_update (collection, MONGOC_UPDATE_NONE, query, update, NULL, &error))
   {
      printf ("%s\n", error.message);
      exit(0);
   }
   printf("\n");
}

// free memory
mongoc_database_destroy(database);//
bson_destroy (query);
mongoc_cursor_destroy (cursor);//
mongoc_collection_destroy (collection);//
mongoc_client_destroy (client);//
mongoc_cleanup ();

return 0;
}


float optimal_agents (float **flowrate, float **distances, int N, float agent_speed, float load_time, float unload_time, float capacity, float availability, float traffic_factor, float operator_efficiency)
{
    // Overview of parameters:
    
    // flowrate - flowrate matrix (NxN size) represending the flowrate between stations
    // distances - distances matrix (NxN size) represending the distances between stations
    // N - matrix dimmension
    // agent_speed - agent velocity [m/min]
    // load_time - time to load at load station [min]
    // unload_time - time to unload at unload station [min]
    // capacity - agent capacity {pcs}
    // availability - availability factor (0-1)
    // traffic_factor - traffic factor (0-1)
    // operator_efficiency - operator efficiency (0-1)
    
    // variables to read out matrix elements
    int i, j;
    
    // total number of required deliveries
    float required_delieveries = 0.0;
    // numerator and denumerator for calculations
    float num, den;
    
    // Le - distance the agent travels empty until the start of the next delivery cycle [m]
    // Ld - distance the agent travels between load and unload station [m]
    // WL - Ideal cycle time per delivery per agent is [min / hr]
    // AT - Available time per hour per agent [min/hr per agent]
    // Tc - delivery cycle time [min/del]
    // ONA - optimal number of agents (float data type)
    float Le, Ld, WL, AT, Tc, ONA;
    
    // Result of optimization - rounded value of optimal number of agents
    int opt_num_of_agents;
    
    // the required deliveries is the sum of all non negative elements of flowrate matrix
    // negative value (use -1) represents that the empty agent is on the way to start station
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            if (flowrate[i][j]>0)
            	{
            	  required_delieveries = required_delieveries + flowrate[i][j];
            	}
        }
    }
    
    // Calculation of Le
    num = 0.0;
    den = 0.0;
    Le = 0.0;
    
    for (i = 0; i<N; i++)
     {
	for (j = 0; j<N; j++)
	 {
	   // Positive flow rate => normal rate, negative flow rate => indicates the return point
	   if (flowrate[i][j] > 0)
	    {
	     num = num + flowrate[i][j] * distances[i][j];
	     den = den + flowrate[i][j];
	    }
	   else
	    {
	     Le =  Le + distances[i][j];
	    }
	  }
     }
    // Calculation of Ld
    Ld = (num / capacity ) / den;

    // Calculation of Tc
    Tc = load_time + Ld/agent_speed + unload_time + Le/agent_speed;
    printf ("Ideal cycle time per delivery per vehicle is [min] %.3lf\n", Tc);

    WL = required_delieveries * Tc;
    printf ("Ideal cycle time per delivery per vehicle is [min / hr] %.3lf\n", WL);

    AT = 60 * availability * traffic_factor * operator_efficiency;
    printf ("Available time per hour per vehicle [min/hr per vehicle] %.3lf\n", AT);

    ONA = WL / AT;
    printf ("Number of required vehicles %.3lf\n", ONA);

    opt_num_of_agents = ceil (ONA);
    printf ("Rounded number of required vehicles %d\n", opt_num_of_agents);
    
    // return the float value of optimal number of agents
    return ONA;
}
