from pythermalcomfort.models import pmv_ppd
from pythermalcomfort.utilities import v_relative, clo_dynamic
from pythermalcomfort.utilities import met_typical_tasks
from pythermalcomfort.utilities import clo_individual_garments

tdb = 27
tr = 25
v = 0.1  # average air speed, [m/s]
rh = 50
activity = "Typing"
garments = ["Sweatpants", "T-shirt", "Shoes or sandals"]

met = met_typical_tasks[activity]
icl = sum(
    [clo_individual_garments[item] for item in garments]
)

vr = v_relative(v=v, met=met)
clo = clo_dynamic(clo=icl, met=met)

results = pmv_ppd(tdb=tdb, tr=tr, vr=vr, rh=rh, met=met, clo=clo, standard="ASHRAE")

print(results)
print(f"pmv={results['pmv']}, ppd={results['ppd']}%")
