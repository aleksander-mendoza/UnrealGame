import os
import sys

import cv2
import numpy as np
import bpy
import bmesh

BOT_ARM_TRANS = [-0.072266 - 0.006897, 0.085937 + 0.009853]
TOP_ARM_TRANS = [0.043945, 0.006836]
RIGHT_LEG_TRANS = -0.014389
BODY_TRANS = [0.0, 0.170266]
LIP_TRANS = [1/4+1/16, -1/32]
LEFT_LEG_COLOR = (64 * 256 + 24) * 256 + 126
RIGHT_LEG_COLOR = (42 * 256 + 126) * 256 + 24
BOT_ARM_COLOR = (62 * 256 + 21) * 256 + 211
LIP_COLOR = (21 * 256 + 109) * 256 + 211
TOP_ARM_COLOR = (255 * 256 + 0) * 256 + 0
BODY_COLOR = (21 * 256 + 211) * 256 + 91
HEAD_COLOR = (204 * 256 + 162) * 256 + 20


class DazOptimizer:

    def __init__(self, workdir=None, name=None):
        if workdir is None:
            workdir = os.path.dirname(bpy.data.filepath)
        if name is None:
            name = os.path.basename(bpy.data.filepath)
            name = name[:name.rindex('.')]
        self.name = name
        self.workdir = workdir

    def gold_palace_dir(self):
        return os.path.join(self.workdir, "textures/original/meipex/m_goldenpalace/g9")

    def us_mask_path(self):
        return os.path.join(self.workdir, "uv_region_mask.png")

    def textures_dir(self):
        return os.path.join(self.workdir, "textures")

    def genesis_dir(self):
        return os.path.join(self.workdir, "textures/original/daz/characters/genesis9")

    def get_uv_mask(self):
        from PIL import Image
        uv_region_mask = np.array(Image.open(self.us_mask_path()), dtype=np.uint32)
        uv_region_mask = (uv_region_mask[:, :, 0] * 256 + uv_region_mask[:, :, 1]) * 256 + uv_region_mask[:, :, 2]
        return uv_region_mask

    def concat_textures(self):
        from PIL import Image
        import re

        name_pattern = re.compile(
            "([A-Za-z0-9_-]+)_(Head|Eyelashes|Legs|Nails|Body|Arms|Mouth)[A-Z]?_(D|NM|R|SSS|C)_[0-9]+\.jpg")

        img_file_paths = {}
        gdir = self.genesis_dir()
        for subdir in os.listdir(gdir):
            subdir = os.path.join(gdir, subdir)
            if os.path.isdir(subdir):
                for file_name in os.listdir(subdir):
                    match = name_pattern.fullmatch(file_name)
                    if match:
                        file_path = os.path.join(subdir, file_name)
                        object_name = match.group(1)
                        body_part = match.group(2)
                        map_type = match.group(3)

                        if body_part not in img_file_paths:
                            img_file_paths[body_part] = {}
                        img_file_paths[body_part][map_type] = file_path

        gp_dir = self.gold_palace_dir()
        if os.path.exists(gp_dir):
            gp_map_types = {
                "Roughness": "R",
                "NormalMap": "NM",
                "Speculairty": "SSS",
                "Color": "D",
            }
            img_file_paths["G9GP"] = {}
            for file_name in gp_dir:
                if file_name.startswith("G9GP_") and file_name.endswith(".jpg"):
                    map_type = file_name[len("G9GP_"):-len(".jpg")]
                    if map_type in gp_map_types:
                        map_type = gp_map_types[map_type]
                        file_path = os.path.join(gp_dir, file_name)
                        img_file_paths["G9GP"][map_type] = file_path

        uv_region_mask = self.get_uv_mask()
        MASK_TILE_SIZE = uv_region_mask.shape[0] // 2
        arms_region_mask = uv_region_mask[:MASK_TILE_SIZE, MASK_TILE_SIZE:]
        body_region_mask = uv_region_mask[MASK_TILE_SIZE:, MASK_TILE_SIZE:]
        legs_region_mask = uv_region_mask[:MASK_TILE_SIZE, :MASK_TILE_SIZE]
        head_region_mask = uv_region_mask[MASK_TILE_SIZE:, :MASK_TILE_SIZE]
        print(img_file_paths)
        # from matplotlib import pyplot as plt
        for map_type in ["R", "NM", "SSS", "D"]:
            head_file: str = img_file_paths['Head'][map_type]
            _, extension = head_file.rsplit('.', maxsplit=1)
            head_tile = Image.open(head_file)
            body_tile = Image.open(img_file_paths['Body'][map_type])
            arms_tile = Image.open(img_file_paths['Arms'][map_type])
            legs_tile = Image.open(img_file_paths['Legs'][map_type])

            head_tile = np.array(head_tile)
            body_tile = np.array(body_tile)
            arms_tile = np.array(arms_tile)
            legs_tile = np.array(legs_tile)
            s = legs_tile.shape[0]
            s2 = s * 2
            if map_type != "NM":
                nails_tile = Image.open(img_file_paths['Nails'][map_type])
                nails_tile = np.array(nails_tile)
                nails_tile_size = s2//8
                if map_type == "R":
                    nails_tile = np.average(nails_tile, axis=2)
                nails_tile = cv2.resize(nails_tile, (nails_tile_size, nails_tile_size))

            else:
                nails_tile = None

            def shift_img(img: np.ndarray, y0, y1, x0, x1, mask: np.ndarray, translation: [float, float], hflip=False):
                shape = [s2, s2, legs_tile.shape[2]] if len(legs_tile.shape) > 2 else [s2, s2]
                new_img = np.zeros(shape, dtype=legs_tile.dtype)
                if hflip:
                    mask = np.flipud(mask)
                    img = np.flipud(img)
                new_img[y0:y1, x0:x1][mask] = img[mask]
                x, y = np.int32(np.array(translation) * s2)
                if x != 0 or y != 0:
                    new_img = np.roll(new_img, [-y, x], axis=[0, 1])
                return new_img

            # Textures are concatenated as follows:
            #   Legs | Arms
            #  ------+-----
            #   Head | Body
            packed = shift_img(arms_tile, 0, s, s, s2, arms_region_mask == BOT_ARM_COLOR, BOT_ARM_TRANS)
            packed += shift_img(arms_tile, 0, s, s, s2, arms_region_mask == TOP_ARM_COLOR, TOP_ARM_TRANS)
            packed += shift_img(legs_tile, 0, s, 0, s, legs_region_mask == LEFT_LEG_COLOR, [0, 0])
            packed += shift_img(legs_tile, 0, s, 0, s, legs_region_mask == RIGHT_LEG_COLOR, [RIGHT_LEG_TRANS, 0], True)
            packed += shift_img(body_tile, s, s2, s, s2, body_region_mask == BODY_COLOR, BODY_TRANS)
            packed += shift_img(head_tile, s, s2, 0, s, head_region_mask == LIP_COLOR, LIP_TRANS)
            # packed += shift_img(head, s, s2, s, s2, head_region_mask == HEAD_COLOR, [0.008526, 0.019377])
            packed[s:, :s] = head_tile
            if nails_tile is not None:
                packed[s2-nails_tile_size:s2, s:s+nails_tile_size] = nails_tile

            # packed[:s, :s] = legs_tile
            # packed[s:, s:] = body_tile
            # packed[:s, s:] = arms_tile
            packed = Image.fromarray(packed)
            packed.save(os.path.join(self.workdir, self.name + '_' + map_type + '.png'))
            # plt.imshow(packed)
            # plt.show()

    def get_body_mesh(self):
        return bpy.data.objects['Tara 9 Mesh']

    def merge_geografts(self):

        GOLD_PAL_M = bpy.data.objects['GoldenPalace_G9 Mesh']
        BODY_M = self.get_body_mesh()
        GOLD_PAL_RIG = bpy.data.objects['GoldenPalace_G9']
        BODY_RIG = bpy.data.objects['Tara 9']

        # merge meshes
        bpy.ops.object.select_all(action='DESELECT')
        GOLD_PAL_M.select_set(True)
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.daz.merge_geografts()

        # merge bones
        bpy.ops.object.select_all(action='DESELECT')
        GOLD_PAL_RIG.select_set(True)
        BODY_RIG.select_set(True)
        bpy.context.view_layer.objects.active = BODY_RIG
        bpy.ops.object.join()

    def get_base_uv_layer(self):
        return self.get_body_mesh().data.uv_layers['Base Multi UDIM']

    def get_base_uv_layer_np(self):
        return np.array([v.uv for v in self.get_base_uv_layer().data])

    def get_base_uv_layer_selection_np(self):
        return np.array([v.select for v in self.get_base_uv_layer().data], dtype=bool)

    def update_base_uv_layer(self, base_layer_np: np.ndarray):
        for v, new_uv in zip(self.get_base_uv_layer().data, base_layer_np):
            v.uv = new_uv

    def pack_uvs(self):

        # ========= Concat UVs =========
        BODY_M = self.get_body_mesh()
        # pack UVs
        bpy.ops.object.select_all(action='DESELECT')
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.object.mode_set(mode='OBJECT')
        base_layer_np = self.get_base_uv_layer_np()
        is_nails = 4 < base_layer_np[:, 0]
        base_layer_np *= 0.5
        out_of_bounds = base_layer_np[:, 0] > 1
        base_layer_np[out_of_bounds] += [-1, 0.5]
        gp_layer = BODY_M.data.uv_layers['Golden Palace']
        gp_layer_np = np.array([v.uv for v in gp_layer.data])
        gp_layer_np *= 0.5
        is_gp = np.all(gp_layer_np > 0, axis=1)
        gp_layer_np[:, 0] += 1
        base_layer_np[is_gp] = gp_layer_np[is_gp]
        uv_mask = self.get_uv_mask()
        uv_mask_size = uv_mask.shape[0]

        def get_pixel_class():
            pixel_coords = np.copy(base_layer_np)
            pixel_coords[:, 1] = 1 - pixel_coords[:, 1]
            pixel_coords = (pixel_coords * uv_mask_size).clip(0, uv_mask_size - 1)
            pixel_coords = np.int32(pixel_coords)
            pix_cls = uv_mask[pixel_coords[:, 1], pixel_coords[:, 0]]
            return pix_cls

        pixel_class = get_pixel_class()

        base_layer_np[pixel_class == BOT_ARM_COLOR] += BOT_ARM_TRANS
        base_layer_np[pixel_class == TOP_ARM_COLOR] += TOP_ARM_TRANS
        base_layer_np[pixel_class == RIGHT_LEG_COLOR, 1] = 1.5 - base_layer_np[pixel_class == RIGHT_LEG_COLOR, 1]
        base_layer_np[pixel_class == RIGHT_LEG_COLOR, 0] += RIGHT_LEG_TRANS
        base_layer_np[pixel_class == BODY_COLOR] += BODY_TRANS
        base_layer_np[is_nails] = base_layer_np[is_nails] / 4 + [0.25, -0.5 / 4]
        self.update_base_uv_layer(base_layer_np)

        # ========= Separate lip UVs =========
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.context.scene.tool_settings.use_uv_select_sync = False
        bpy.ops.uv.select_all(action='DESELECT')
        bpy.ops.mesh.select_all(action='DESELECT')

        me = bpy.context.object.data
        bm = bmesh.from_edit_mesh(me)
        uv_layer = bm.loops.layers.uv.verify()

        # for v in bm.verts:
        #    v.select_set(False)

        for face in bm.faces:
            full_loop = True
            for loop in face.loops:
                loop_uv = loop[uv_layer]
                uv = np.array(loop_uv.uv)
                uv[1] = 1 - uv[1]
                pixel_coord = (uv * uv_mask_size).clip(0, uv_mask_size - 1)
                pixel_coord = np.int32(pixel_coord)
                pixel_cls = uv_mask[pixel_coord[1], pixel_coord[0]]
                matched = pixel_cls == LIP_COLOR
                # loop_uv.select = matched
                full_loop = full_loop and matched
                # if matched:
                #    loop.vert.select_set(True)
            face.select_set(full_loop)

        # bm.select_mode = {'VERT', 'EDGE', 'FACE'}
        bm.select_flush_mode()
        # bpy.context.tool_settings.mesh_select_mode = (False, False, True)
        bpy.ops.uv.select_all(action='SELECT')
        bpy.ops.mesh.select_all(action='SELECT')
        bpy.ops.uv.select_split()

        base_layer_np = self.get_base_uv_layer_np()
        selection = self.get_base_uv_layer_selection_np()
        base_layer_np[selection] = base_layer_np[selection] / 4 + LIP_TRANS
        self.update_base_uv_layer(base_layer_np)
        # += [0.043945, 0.006836] # top arm
        # += [-0.072266 , 0.085937] # obttom arm
        # += [0.008526, 0.019377] # torso
        # *= 0.25# nails
        # -= 0.5 # nails

    def remove_default_cube(self):
        for x in list(bpy.data.objects):
            bpy.data.objects.remove(x)
        for x in list(bpy.data.collections):
            bpy.data.collections.remove(x)

        # save file

    # *= 0.25# nails
    # -= 0.5 # nails

    def finalize(self):
        self.merge_geografts()
        self.pack_uvs()


def save_textures(duf_filepath):
    duf_filepath = os.path.abspath(duf_filepath)
    workdir = os.path.dirname(duf_filepath)
    name = os.path.basename(duf_filepath)[:-len(".duf")]
    blend_filepath = os.path.join(workdir, name + ".blend")
    bpy.ops.wm.save_as_mainfile(filepath=blend_filepath)
    bpy.ops.daz.save_local_textures()


def install_libraries():
    pil_missing = False
    opencv_missing = False
    try:
        import PIL
    except ModuleNotFoundError:
        pil_missing = True
    try:
        import cv2
    except ModuleNotFoundError:
        opencv_missing = True
    if pil_missing or opencv_missing:
        py_exe = sys.executable
        res_path = os.path.realpath(os.path.join(py_exe, "../../lib/site-packages"))
        print("Run the following command as an admin: ")
        print('&"' + py_exe + '" -m pip install pillow opencv-python "--target=' + res_path + '"')


def run_outside_blender():
    import subprocess

    file_path = os.path.realpath(__file__)

    if len(sys.argv) < 2 or not sys.argv[1].endswith(".duf"):
        print("Specify path to .duf")
        exit()
    duf_path: str = os.path.abspath(sys.argv[1])
    if not os.path.exists(duf_path):
        print("File not exists:", duf_path)
        exit()
    subprocess.run(["blender", "-P", file_path, "--", duf_path])

    # Image("")


bl_info = {
    "name": "Daz Optimizer",
    "blender": (2, 80, 0),
    "category": "Object",
}


class EasyImportPanel(bpy.types.Panel):
    bl_label = "Panel"
    bl_idname = "dazoptim_easy_import_panel"
    bl_space_type = 'FILE_BROWSER'
    bl_region_type = 'TOOLS'
    filepath = None
    directory = None

    @classmethod
    def poll(cls, context):
        op = context.active_operator
        print("aaa=", op.bl_idname)
        if op and op.bl_idname == "DAZ_OT_easy_import_daz":
            cls.directory = op.directory
            cls.filepath = op.filepath
        return False

    # needs a draw method
    def draw(self, context):
        pass


class DazLoad_operator(bpy.types.Operator):
    """ Load daz character """
    bl_idname = "dazoptim.load"
    bl_label = "Load Daz character"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        bpy.ops.daz.easy_import_daz('INVOKE_DEFAULT',
                                    # filepath=self.duf_path,
                                    # files=[filepath],
                                    # directory="",
                                    filter_glob="*.duf;*.dsf;*.png;*.jpeg;*.jpg;*.bmp",
                                    fitMeshes='DBZFILE',
                                    materialMethod='EXTENDED_PRINCIPLED',
                                    useMergeMaterials=True,
                                    useEliminateEmpties=True,
                                    useMergeRigs=False,
                                    useApplyTransforms=False,
                                    useMergeToes=False,
                                    useFavoMorphs=False,
                                    useUnits=False,
                                    useExpressions=False,
                                    useVisemes=False,
                                    useHead=False,
                                    useFacs=False,
                                    useFacsdetails=False,
                                    useFacsexpr=False,
                                    useAnime=False,
                                    usePowerpose=False,
                                    useBody=False,
                                    useBulges=False,
                                    useJcms=True,
                                    ignoreFingers=True,
                                    useMasculine=False,
                                    useFeminine=False,
                                    useFlexions=True,
                                    useBakedCorrectives=True,
                                    useDazFavorites=True,
                                    useAdjusters=False,
                                    onMorphSuffix='SMART',
                                    useTransferFace=True,
                                    useTransferHair=False,
                                    useTransferGeografts=True,
                                    useTransferClothes=True,
                                    useTransferHD=True,
                                    useMergeGeografts=False,
                                    useMakePosable=True,
                                    useFinalOptimization=False,
                                    ignoreUrl=False,
                                    ignoreFinger=False,
                                    morphSuffix="",
                                    ignoreHdMorphs=False,
                                    useMhxOnly=False,
                                    duplicateDistance=1,
                                    useMergeNonConforming='CONTROLS',
                                    useConvertWidgets=True,
                                    useHiddenRigs=False,
                                    useMergeUvs=True,
                                    allowOverlap=False,
                                    keepOriginal=False,
                                    useFixTiles=True,
                                    useSubDDisplacement=True,
                                    useGeoNodes=False,
                                    morphStrength=1,
                                    skinColor=(0.6, 0.4, 0.25, 1),
                                    clothesColor=(0.09, 0.01, 0.015, 1),
                                    useApplyRestPoses=True,
                                    favoPath="")
        return {'FINISHED'}


class DazSave_operator(bpy.types.Operator):
    """ Load daz character """
    bl_idname = "dazoptim.save"
    bl_label = "Load Daz character"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        if bpy.types.dazoptim_easy_import_panel.filepath is None:
            self.report({"WARNING"}, "Load a DAZ character first!")
            return {'CANCELLED'}
        save_textures(bpy.types.dazoptim_easy_import_panel.filepath)
        return {'FINISHED'}


class DazConcatTextures_operator(bpy.types.Operator):
    """ Concatenate textures into one """
    bl_idname = "dazoptim.concat"
    bl_label = "Concatenate textures"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().concat_textures()

        return {'FINISHED'}


class DazMergeMeshes_operator(bpy.types.Operator):
    """ Merge Meshes """
    bl_idname = "dazoptim.merge_meshes"
    bl_label = "Merge meshes"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().merge_geografts()

        return {'FINISHED'}


class DazOptimizeUVs_operator(bpy.types.Operator):
    """ Optimize UVs """
    bl_idname = "dazoptim.optim_uvs"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazAddBreastBones_operator(bpy.types.Operator):
    """ Add breast bones """
    bl_idname = "dazoptim.breast_bones"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().separate_lips()

        return {'FINISHED'}


class DazAddGluteBones_operator(bpy.types.Operator):
    """ Add glute bones """
    bl_idname = "dazoptim.glute_bones"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazAddThighBones_operator(bpy.types.Operator):
    """ Add thigh bones """
    bl_idname = "dazoptim.thigh_bones"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazFitClothes_operator(bpy.types.Operator):
    """ fit clothes """
    bl_idname = "dazoptim.fit_clothes"
    bl_label = "Fit clothes"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazOptimizeHair_operator(bpy.types.Operator):
    """ Optimize hair """
    bl_idname = "dazoptim.optim_hair"
    bl_label = "Optimize Hair"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazConvertToUe5Skeleton_operator(bpy.types.Operator):
    """ Convert rig to UE5-compatible skeleton """
    bl_idname = "dazoptim.convert_ue5"
    bl_label = "Convert rig to UE5-compatible skeleton"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazSendToUnreal(bpy.types.Operator):
    """ Send to unreal """
    bl_idname = "dazoptim.send_to_ue5"
    bl_label = "Send to unreal"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazOptimize_sidebar(bpy.types.Panel):
    """DazOptim actions"""
    bl_label = "DazOptim"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "DazOptim"

    def draw(self, context):
        col = self.layout.column(align=True)
        prop = col.operator(DazLoad_operator.bl_idname, text="1. Load Daz")
        prop = col.operator(DazSave_operator.bl_idname, text="2. Save textures")
        prop = col.operator(DazConcatTextures_operator.bl_idname, text="3. Merge textures")
        prop = col.operator(DazMergeMeshes_operator.bl_idname, text="4. Merge Meshes")
        prop = col.operator(DazOptimizeUVs_operator.bl_idname, text="5. Optimize UVs")
        prop = col.operator(DazAddBreastBones_operator.bl_idname, text="6. Add breast bones")
        prop = col.operator(DazAddGluteBones_operator.bl_idname, text="7. Add glute bones")
        prop = col.operator(DazAddThighBones_operator.bl_idname, text="8. Add thigh bones")
        prop = col.operator(DazFitClothes_operator.bl_idname, text="9. Fit Clothes")
        prop = col.operator(DazOptimizeHair_operator.bl_idname, text="10. Optimize hair")
        prop = col.operator(DazConvertToUe5Skeleton_operator.bl_idname, text="11. Convert to UE5 Skeleton")
        prop = col.operator(DazSendToUnreal.bl_idname, text="12. Send to unreal")


classes = [
    DazOptimize_sidebar,
    DazLoad_operator,
    DazSave_operator,
    DazConcatTextures_operator,
    DazOptimizeUVs_operator,
    DazMergeMeshes_operator,
    DazAddBreastBones_operator,
    DazAddGluteBones_operator,
    DazAddThighBones_operator,
    DazFitClothes_operator,
    DazOptimizeHair_operator,
    DazConvertToUe5Skeleton_operator,
    DazSendToUnreal,
    EasyImportPanel,

]


def register():
    for c in classes:
        bpy.utils.register_class(c)


def unregister():
    for c in classes:
        bpy.utils.unregister_class(c)


if __name__ == '__main__':
    register()
