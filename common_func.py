# # Import libraries and define functions

# In[1]:

import sys
import time
import pickle
import glob
import itertools

import numpy as np
import uproot
import pandas as pd
from scipy import interpolate
from scipy.interpolate import interp1d

# In[2]:


def read_ROOT(fileName, treeName, key_list = None):

    # open the file using uproot
    file = uproot.open(fileName)
    # get list of keys
    if key_list == None:
        key_list = file[treeName].keys()

    # define dictionary
    hits_dict = {k:[] for k in key_list}

    # loop over all keys and add to dictionary 
    for keyIN in key_list:
        hits_dict[keyIN]=file["Hit Data"][keyIN].array(library="np")
        #print(keyIN)
    
    return hits_dict


# In[3]:


def append_value(dict_obj, key, value):
    
    # Check if key exist in dict or not
    if key in dict_obj:
        # Key exist in dict.
        # Check if type of value of key is list or not
        if not isinstance(dict_obj[key], list):
            # If type is not list then make it list
            dict_obj[key] = [dict_obj[key]]
        # Append the value in list
        dict_obj[key].append(np.array(value))
    else:
        # As key is not in dict,
        # so, add key-value pair
        dict_obj[key] = np.array(value)


# In[4]:


def neutron_weight(energy):
    weight = np.piecewise(energy, [energy < 1, (energy >= 1) * (energy <= 50),  energy > 50], \
[lambda energy: 2.5+18.2*np.exp(-np.log(energy)**2/6),lambda energy: 5.0+17*np.exp(-np.log(2*energy)**2/6),\
lambda energy: 2.5+3.25*np.exp(-np.log(0.04*energy)**2/6)])
    return weight


# In[5]:


def calculate_dose(KE_array, ptype_array, dict_eff_dose):

    # define array to save the dose for each particle type
    dose_list=[]
    particle_list = []
    index_list = []

    # loop over all keys
    for keyIN in dict_eff_dose.keys():

        # find the index for each particle type
        particle_ind = np.argwhere(ptype_array==keyIN).flatten()
        
        # continue if there are no particles of that type 
        if np.size(particle_ind) == 0:
            continue 
            
        # interpolate data to make even step sizes
        interp_func = interpolate.interp1d(np.log10(np.array(dict_eff_dose[keyIN][0], dtype=float)),\
 np.log10(np.array(dict_eff_dose[keyIN][1], dtype=float)), fill_value= 'extrapolate', kind='linear')

        # interpolate the dose conversion for our given energy
        KE_array_nan = KE_array[particle_ind]
        KE_array_nan[KE_array_nan == 0] = 'nan' 
        dose_interp = 10**interp_func(np.log10(KE_array_nan))

        # add the radiation weighting factor
        if keyIN == 'gamma' or keyIN =='e-' or keyIN == 'e+': 
            weight = 1
        elif keyIN =='proton':
            weight = 2
        elif keyIN =='neutron':
            weight = neutron_weight(KE_array[particle_ind])
        else: # heavy ions
            weight = 20

        # append the dose amount
        dose_list.append(dose_interp*weight)
        particle_list.append(keyIN)
        index_list.append(particle_ind)
    
    return dose_list, particle_list,index_list 


def calculate_density(wt):

    BN_density = 2.1 #unit: g/cm3
    HDPE_density=0.96876

    mix_density = 1/((1-wt)/HDPE_density+wt/BN_density)
    
    return mix_density