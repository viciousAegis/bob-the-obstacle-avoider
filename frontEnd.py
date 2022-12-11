import streamlit as st
import matplotlib.pyplot as plt
from time import sleep

import requests
import json
import urllib.request
from time import sleep
from oneM2M_functions import *
import urllib3

print('\033[96m')

http = urllib3.PoolManager(num_poopythls=3)

uri_cse = "http://esw-onem2m.iiit.ac.in:443/~/in-cse/in-name"

ae1 = "Team-30"
cnt1 = "Node-1"
cnt2 = "Data"
cnt3 = "coordinates"

uri_ae_1 = uri_cse + "/" + ae1

uri_cnt_1 = uri_ae_1 + "/" + cnt1 + "/" + cnt2 + "/" + cnt3 + "/la" # get latest instance

headers = {
'X-M2M-Origin': 'cYRadM:Mi1kta',
'Content-type': 'application/json;ty=4'
}

x  = []
y = []

st.set_page_config(layout="centered", page_title="BOB", page_icon=":car:")


"""
# BOB the obstacle avoider
***
"""
plot_spot = st.empty()

while 1:
    try:
        response_1 = requests.get(uri_cnt_1, headers=headers)

    except:
        print("failed to access oneM2M\n")
        exit()
        
    data_rec = json.loads(response_1._content)
    data_list = data_rec['m2m:cin']['con']

    data_list = data_list[1:-1].split(',')

    x_coord = data_list[1]
    y_coord = data_list[2]

    print(x_coord, y_coord)

    x.append(float(x_coord))
    y.append(float(y_coord))

    for i in range(0, len(x)):
        try:
            plt.plot(x[i:i+2], y[i:i+2], 'co-', linewidth=0.5, marker='.')
        except:
            pass

    # plt.tick_params(axis='x', which='both', bottom=False, top=False, labelbottom=False)
    # plt.tick_params(axis='y', which='both', bottom=False, top=False, labelbottom=False)


    # for pos in ['right', 'top', 'bottom', 'left']:
    #     plt.gca().axes.get_xaxis().set_visible(False)
    #     plt.gca().axes.get_yaxis().set_visible(False)
    #     # plt.gca().spines[pos].set_visible(False)

    ax_range = 100
    plt.xlim(-ax_range, ax_range)
    plt.ylim(-ax_range, ax_range)

    plt.grid()
    fig1 = plt.show()
    st.set_option('deprecation.showPyplotGlobalUse', False)
    # st.pyplot(fig1)

    with plot_spot:
        st.pyplot(fig1)

    sleep(1.5)

