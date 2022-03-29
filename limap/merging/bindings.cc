#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
#include <pybind11/stl_bind.h>

#include <vector>
#include <Eigen/Core>
#include "_limap/helpers.h"
#include "base/linebase.h"
#include "base/linetrack.h"
#include "base/line_linker.h"

#include "merging/merging_utils.h"
#include "merging/merging.h"

namespace py = pybind11;

namespace limap {

void bind_merging_core(py::module &m) {
    using namespace merging;
    m.def("_MergeToLineTracks", 
        [] (Graph& graph,
            const std::vector<std::vector<Line2d>>& all_lines_2d, 
            const std::vector<CameraView>& views,
            const std::vector<std::vector<Line3d>>& all_lines_3d,
            const std::vector<std::vector<int>>& neighbors,
            LineLinker linker) 
        {
            std::vector<LineTrack> linetracks;
            MergeToLineTracks(graph, linetracks, all_lines_2d, views, all_lines_3d, neighbors, linker);
            return linetracks;
        }
    );

    m.def("_RemergeLineTracks", 
            [] (const std::vector<LineTrack>& linetracks,
                LineLinker3d linker3d,
                const int num_outliers)
            {
                return RemergeLineTracks(linetracks, linker3d, num_outliers);
            },
            py::arg("linetracks"),
            py::arg("linker3d"),
            py::arg("num_outliers") = 2
    );
}

void bind_merging_utils(py::module& m) {
    using namespace merging;
    m.def("_GetLines", 
        [] (const std::vector<Eigen::MatrixXd>& segs3d) {
            std::vector<Line3d> lines;
            GetLines(segs3d, lines);
            return lines;
        }
    );

    m.def("_GetSegs3d", 
        [] (const std::vector<Line3d>& lines) {
            std::vector<Eigen::MatrixXd> segs3d;
            GetSegs3d(lines, segs3d);
            return segs3d;
        }
    );

    m.def("_SetUncertaintySegs3d", &SetUncertaintySegs3d);

    m.def("_CheckReprojection", [] (const LineTrack& linetrack, const std::vector<CameraView>& views, const double& th_angular2d, const double& th_perp2d) 
        {
            std::vector<bool> results;
            CheckReprojection(results, linetrack, views, th_angular2d, th_perp2d);
            return results;
        }
    );
    m.def("_FilterSupportLines", [] (const std::vector<LineTrack>& linetracks, const std::vector<CameraView>& views, const double& th_angular2d, const double& th_perp2d, const int num_outliers)
        {
            std::vector<LineTrack> new_linetracks;
            FilterSupportingLines(new_linetracks, linetracks, views, th_angular2d, th_perp2d, num_outliers);
            return new_linetracks;
        },
        py::arg("linetracks"),
        py::arg("views"),
        py::arg("th_angular2d"),
        py::arg("th_perp2d"),
        py::arg("num_outliers") = 2
    );

    m.def("_CheckSensitivity", [] (const LineTrack& linetrack, const std::vector<CameraView>& views, const double& th_angular3d)
        {
            std::vector<bool> results;
            CheckSensitivity(results, linetrack, views, th_angular3d);
            return results;
        }
    );
    m.def("_FilterTracksBySensitivity", [] (const std::vector<LineTrack>& linetracks, const std::vector<CameraView>& views, const double& th_angular3d, const int& min_support_ns)
        {
            std::vector<LineTrack> newtracks;
            FilterTracksBySensitivity(newtracks, linetracks, views, th_angular3d, min_support_ns);
            return newtracks;
        }
    );
    m.def("_FilterTracksByOverlap", [] (const std::vector<LineTrack>& linetracks, const std::vector<CameraView>& views, const double& th_overlap, const int& min_support_ns)
        {
            std::vector<LineTrack> newtracks;
            FilterTracksByOverlap(newtracks, linetracks, views, th_overlap, min_support_ns);
            return newtracks;
        }
    );
}

void bind_merging(py::module& m) {
    bind_merging_core(m);
    bind_merging_utils(m);
}

} // namespace limap
